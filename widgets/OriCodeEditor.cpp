#include "OriCodeEditor.h"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QToolTip>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextDocumentFragment>

Q_DECLARE_METATYPE(QTextDocumentFragment)

#define FOLDED_TEXT_TYPE (QTextFormat::UserObject+2)
#define FOLDED_TEXT_PROP 2

//------------------------------------------------------------------------------
//                               Helpers
//------------------------------------------------------------------------------

namespace {

struct LineParts { QStringRef indent; QStringRef text; };

LineParts splitLine(const QString &line)
{
    int i = 0;
    while (i < line.length() && (line[i] == ' ' || line[i] == '\t')) i++;
    return { line.leftRef(i), line.midRef(i) };
}

inline bool isEmpty(const QString &line)
{
    return QStringRef(&line).trimmed().isEmpty();
}

bool isCursorInIndent(const QTextCursor &cursor)
{
    return cursor.positionInBlock() <= splitLine(cursor.block().text()).indent.length();
}

bool hasMultilineSelection(const QTextCursor &cursor)
{
    auto startBlock = cursor.document()->findBlock(cursor.selectionStart());
    auto endBlock = cursor.document()->findBlock(cursor.selectionEnd());
    return endBlock.blockNumber() > startBlock.blockNumber();
}

struct SelectedRange
{
    SelectedRange(Ori::Widgets::CodeEditor *editor): editor(editor)
    {
        cursor = editor->textCursor();

        startPos = cursor.selectionStart();
        QTextCursor startCursor(editor->document());
        startCursor.setPosition(startPos);
        startBlock = startCursor.blockNumber();
    
        endPos = cursor.selectionEnd();
        QTextCursor endCursor(editor->document());
        endCursor.setPosition(endPos);
        endBlock = endCursor.blockNumber();
        
        if (startBlock == endBlock)
            cursor.select(QTextCursor::LineUnderCursor);
        // If selection ends at the beginning of a line, don't include that line
        else if (endCursor.atBlockStart())
            endBlock--;
    }
    
    ~SelectedRange()
    {
        if (startBlock == endBlock) {
            // Restore position in single line
            QTextCursor cursor(editor->document());
            cursor.setPosition(startPos + posDiff);
            if (endPos > startPos)
                cursor.setPosition(endPos + posDiff, QTextCursor::KeepAnchor);
            editor->setTextCursor(cursor);
        } else {
            // Restore selected lines
            QTextBlock start = editor->document()->findBlockByNumber(startBlock);
            QTextBlock end = editor->document()->findBlockByNumber(endBlock);
            if (!start.isValid() || !end.isValid()) return;
            
            QTextCursor cursor(start);
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.setPosition(end.position() + end.length(), QTextCursor::KeepAnchor);
            editor->setTextCursor(cursor);
        }
    }
    
    void modify(std::function<QString(const QString &line)> changeLine)
    {
        cursor.beginEditBlock();
        for (int blockNum = startBlock; blockNum <= endBlock; blockNum++) {
            QTextBlock block = editor->document()->findBlockByNumber(blockNum);
            if (!block.isValid()) continue;
            
            QString line = block.text();
            if (_skipEmptyLines && isEmpty(line)) continue;
            
            QString changedLine = changeLine(line);
            if (changedLine == line) continue;
            
            // For restoring cursor position in single line
            posDiff = changedLine.length() - line.length();

            QTextCursor lineCursor(block);
            lineCursor.select(QTextCursor::LineUnderCursor);
            lineCursor.insertText(changedLine);
        }
        cursor.endEditBlock();
    }
    
    SelectedRange& skipEmptyLines() { _skipEmptyLines = true; return *this; }
    
    Ori::Widgets::CodeEditor *editor;
    QTextCursor cursor;
    int startPos, endPos;
    int startBlock, endBlock;
    int posDiff = 0;
    bool _skipEmptyLines = false;
};

int getIndentLevel(const QString& line, int tabWidth)
{
    int level = 0;
    for (int i = 0; i < line.length(); i++) {
        if (line[i] == ' ') {
            int spaceCount = 0;
            while (i < line.length() && line[i] == ' ') {
                spaceCount++;
                i++;
            }
            i--;
            level += spaceCount / tabWidth;
        } else if (line[i] == '\t') {
            level++;
        } else {
            break;
        }
    }
    return level;
}

struct CodeBlock
{
    /// A position in startLine where the folding should be inserted
    int mountPos = -1;
    int startLine = -1;
    int endLine = -1;
    bool ok() const { return startLine >= 0 && endLine > startLine; }
};

CodeBlock findPythonCodeBlock(QTextBlock &startBlock, int tabWidth)
{
    if (!startBlock.isValid())
        return {};
    
    int mountPos = -1;
    QString line = startBlock.text();
    for (int i = line.length()-1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        if (line[i] == ':') {
            mountPos = i+1;
            break;
        }
    }
    if (mountPos < 0)
        return {};

    int startLevel = getIndentLevel(line, tabWidth);
    int endLine = -1;
    auto block = startBlock.next();
    while (block.isValid()) {
        QString line = block.text();
        if (isEmpty(line)) {
            block = block.next();
            continue;
        }
        if (getIndentLevel(line, tabWidth) <= startLevel)
            break;
        endLine = block.blockNumber();
        block = block.next();
    }
    return { mountPos, startBlock.blockNumber(), endLine };
}

} // namespace

namespace Ori {
namespace Widgets {

//------------------------------------------------------------------------------
//                               LineNumberArea
//------------------------------------------------------------------------------

class EditorLineNums : public QWidget
{
public:
    EditorLineNums(Ori::Widgets::CodeEditor *editor) : QWidget(editor), _editor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(calcWidth(), 0);
    }

    void adjustGeometry(const QRect &editorContentRect)
    {
        QRect r = editorContentRect;
        r.setWidth(calcWidth());
        setGeometry(r);
    }

    int calcWidth() const
    {
        int digits = 1;
        // in a plain text edit each line will consist of one QTextBlock
        int max = qMax(1, _editor->blockCount());
        while (max >= 10)
        {
            max /= 10;
            digits++;
        }
        const CodeEditor::Style &style = _editor->_style;
        return style.lineNumsLeftPadding + style.lineNumsRightPadding + style.lineNumsMargin +
                fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    }
    
protected:
    void paintEvent(QPaintEvent *event) override
    {
        auto r = event->rect();
        int lineNumW = r.width();
        const CodeEditor::Style &style = _editor->_style;
        int borderPos = lineNumW - style.lineNumsMargin;
    
        QPainter painter(this);
        if (style.lineNumsMargin > 0)
            painter.fillRect(r, Qt::white);
        r.setWidth(r.width() - style.lineNumsMargin);
        painter.fillRect(r, style.lineNumsBackColor);
    
        painter.setPen(style.lineNumsBorderColor);
        painter.drawLine(borderPos, r.top(), borderPos, r.bottom());
    
        if (style.lineNumsFontSizeDec > 0) {
            auto f = painter.font();
            f.setPointSize(f.pointSize() - style.lineNumsFontSizeDec);
            painter.setFont(f);
        }
    
        QTextBlock block = _editor->firstVisibleBlock();
        int lineNum = block.blockNumber() + 1;
        int lineTop = qRound(_editor->blockBoundingGeometry(block).translated(_editor->contentOffset()).top());
        int lineH = qRound(_editor->blockBoundingRect(block).height());
        int lineBot = lineTop + lineH;
        while (block.isValid() && lineTop <= r.bottom())
        {
            if (block.isVisible() && lineBot >= r.top())
            {
                if (_editor->_lineHints.contains(lineNum))
                {
                    painter.setPen(style.lineNumsTextColorErr);
                    painter.fillRect(0, lineTop, lineNumW - style.lineNumsMargin, lineH, style.lineNumsBackColorErr);
                }
                else
                    painter.setPen(style.lineNumsTextColor);
                painter.drawText(0, lineTop, lineNumW - style.lineNumsRightPadding - style.lineNumsMargin, lineH,
                    Qt::AlignRight|Qt::AlignVCenter, QString::number(lineNum-1));
            }
            block = block.next();
            lineTop = lineBot;
            lineH = qRound(_editor->blockBoundingRect(block).height());
            lineBot = lineTop + lineH;
            lineNum++;
        }
    }
    
    bool event(QEvent *event) override
    {
        if (event->type() != QEvent::ToolTip)
            return QWidget::event(event);

        auto helpEvent = dynamic_cast<QHelpEvent*>(event);
        if (!helpEvent) return false;

        auto hint = _editor->_lineHints.isEmpty() ? QString() :
            _editor->_lineHints.value(findLineNumber(helpEvent->pos().y()));
        if (hint.isEmpty())
            QToolTip::hideText();
        else
            QToolTip::showText(helpEvent->globalPos(), hint);
        event->accept();
        return true;
    }
    
private:
    Ori::Widgets::CodeEditor *_editor;

    int findLineNumber(int y) const
    {
        QTextBlock block = _editor->firstVisibleBlock();
        int lineNum = block.blockNumber() + 1;
        int lineTop = qRound(_editor->blockBoundingGeometry(block).translated(_editor->contentOffset()).top());
        int lineBot = lineTop + qRound(_editor->blockBoundingRect(block).height());
        while (block.isValid())
        {
            if (y >= lineTop && y < lineBot) {
                return lineNum;
            }
            block = block.next();
            lineTop = lineBot;
            lineBot = lineTop + qRound(_editor->blockBoundingRect(block).height());
            lineNum++;
        }
        return 0;
    }
};

//------------------------------------------------------------------------------
//                               FoldedTextView
//------------------------------------------------------------------------------

QSizeF FoldedTextView::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc)
    Q_UNUSED(posInDocument)
    if (format.type() != format.CharFormat) return {};
    const QTextCharFormat &tf = reinterpret_cast<const QTextCharFormat&>(format);
    return QFontMetrics(tf.font()).boundingRect(QStringLiteral("...")).size();
}

void FoldedTextView::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc)
    Q_UNUSED(posInDocument)
    if (format.type() != format.CharFormat) return;
    painter->drawText(rect, QStringLiteral("..."));
    painter->drawRect(rect);
}

//------------------------------------------------------------------------------
//                                CodeFolder
//------------------------------------------------------------------------------

class CodeFolder
{
public:
    CodeFolder(CodeEditor *editor): editor(editor) {}

    void foldCodeBlock(int tabWidth, CodeEditor::FoldingType foldMode)
    {
        Q_UNUSED(foldMode)
    
        auto block = editor->textCursor().block();
        
        // TODO: add support for more type of blocks when needed
        auto codeBlock = findPythonCodeBlock(block, tabWidth);
        if (!codeBlock.ok())
            return;
        
        QTextCursor foldCursor(editor->document());
        foldCursor.setPosition(block.position() + codeBlock.mountPos);
        QTextBlock endBlock = editor->document()->findBlockByNumber(codeBlock.endLine);
        foldCursor.setPosition(endBlock.position() + endBlock.length() - 1, QTextCursor::KeepAnchor);
        
        foldSelection(foldCursor);
        
        // Position the cursor after the folded block symbol
        QTextCursor newCursor(editor->document());
        newCursor.setPosition(block.position() + codeBlock.mountPos + 1);
        editor->setTextCursor(newCursor);
    }
    
    void foldSelection(QTextCursor c)
    {
        QTextCharFormat f;
        f.setObjectType(FOLDED_TEXT_TYPE);
        f.setProperty(FOLDED_TEXT_PROP, QVariant::fromValue(c.selection()));
        c.insertText(QString(QChar::ObjectReplacementCharacter), f);
    }
    
    void unfold()
    {
        auto c = editor->textCursor();
        if (c.hasSelection())
            return;
        
        auto f = c.charFormat();
        if (f.objectType() != FOLDED_TEXT_TYPE)
            return;
    
        c.movePosition(c.Left, c.KeepAnchor);
        c.insertFragment(f.property(FOLDED_TEXT_PROP).value<QTextDocumentFragment>());
    }
    
    bool hasFoldings() const
    {
        auto block = editor->document()->firstBlock();
        while (block.isValid()) {
            QString text = block.text();
            for (int i = 0; i < text.length(); i++) {
                if (text[i] == QChar::ObjectReplacementCharacter) {
                    QTextCursor cursor(block);
                    cursor.setPosition(block.position() + i);
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                    if (cursor.charFormat().objectType() == FOLDED_TEXT_TYPE)
                        return true;
                }
            }
            block = block.next();
        }
        return false;
    }

    QString toUnfoldedText() const
    {
        QString result;
        QTextStream out(&result);
        QTextBlock block = editor->document()->firstBlock();
        while (block.isValid()) {
            QString line = block.text();
            for (int i = 0; i < line.length(); i++) {
                if (line[i] == QChar::ObjectReplacementCharacter) {
                    QTextCursor cursor(block);
                    cursor.setPosition(block.position() + i);
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                    QTextCharFormat format = cursor.charFormat();
                    if (format.objectType() == FOLDED_TEXT_TYPE) {
                        auto fragment = format.property(FOLDED_TEXT_PROP).value<QTextDocumentFragment>();
                        out << fragment.toPlainText();
                        continue;
                    }
                }
                out << line[i];
            }
            block = block.next();
            if (block.isValid())
                out << "\n";
        }
        return result;
    }
    
    CodeEditor *editor;
};

//------------------------------------------------------------------------------
//                               CodeEditor
//------------------------------------------------------------------------------

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    setWordWrapMode(QTextOption::NoWrap);
    setTabStopDistance(40); // twice less that the default

    _lineNums = new EditorLineNums(this);

    _style.currentLineColor = QColor("steelBlue").lighter(220);
    _style.lineNumsTextColor = Qt::gray;
    _style.lineNumsTextColorErr = Qt::white;
    _style.lineNumsBorderColor = QColor(0xfff0f0f0);
    _style.lineNumsBackColor = QColor(0xfffafafa);
    _style.lineNumsBackColorErr = QColor(Qt::red).lighter(130);
    _style.lineNumsRightPadding = 8;
    _style.lineNumsLeftPadding = 12;
    _style.lineNumsMargin = 4;
    _style.lineNumsFontSizeDec = 2;
    
    document()->documentLayout()->registerHandler(FOLDED_TEXT_TYPE, new FoldedTextView(this)); 

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::onBlockCountChanged);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::onDocUpdateRequest);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    onBlockCountChanged();
    highlightCurrentLine();
}

bool CodeEditor::loadCode(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning() << "Failed to open" << fileName << f.errorString();
        return false;
    }
    setCode(QString::fromUtf8(f.readAll()));
    return true;
}

bool CodeEditor::saveCode(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
        qWarning() << "Failed to open" << fileName << f.errorString();
        return false;
    }
    f.write(code().toUtf8());
    return true;
}

QString CodeEditor::code() const
{
    if (_codeFolder && _codeFolder->hasFoldings())
        return _codeFolder->toUnfoldedText();
    return toPlainText();
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    _lineNums->adjustGeometry(contentsRect());
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    auto key = e->key();
    if (key == Qt::Key_Slash && e->modifiers() == Qt::ControlModifier) {
        toggleCommentSelection();
        return;
    }
    if (_autoIndent && (key == Qt::Key_Return || key == Qt::Key_Enter)) {
        handleSmartEnter();
        return;
    }
    if (_autoIndent && key == Qt::Key_Backspace) {
        auto cursor = textCursor();
        if (!cursor.hasSelection() && isCursorInIndent(cursor) && !cursor.atBlockStart()) {
            unindentSelection();
            return;
        }
    }
    if (key == Qt::Key_Home) {
        handleSmartHome(e->modifiers() == Qt::ShiftModifier);
        return;
    }
    if (key == Qt::Key_Tab || key == Qt::Key_Backtab) {
        if (key == Qt::Key_Backtab || e->modifiers() == Qt::ShiftModifier) {
            unindentSelection();
            return;
        }
        auto cursor = textCursor();
        if (hasMultilineSelection(cursor) || (isCursorInIndent(cursor) && !cursor.hasSelection())) {
            indentSelection();
            return;
        }
    }
    QPlainTextEdit::keyPressEvent(e);
}

void CodeEditor::onBlockCountChanged()
{
    setViewportMargins(_lineNums->calcWidth(), 0, 0, 0);
}

void CodeEditor::onDocUpdateRequest(const QRect &rect, int dy)
{
    if (dy)
        _lineNums->scroll(0, dy);
    else
        _lineNums->update(0, rect.y(), _lineNums->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        setViewportMargins(_lineNums->calcWidth(), 0, 0, 0);
}

void CodeEditor::highlightCurrentLine()
{
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(_style.currentLineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    setExtraSelections({selection});
}

void CodeEditor::setShowWhitespaces(bool on)
{
    QTextOption option =  document()->defaultTextOption();
    auto flags = option.flags();
    flags.setFlag(QTextOption::ShowTabsAndSpaces, on);
    option.setFlags(flags);
    document()->setDefaultTextOption(option);
}

bool CodeEditor::showWhitespaces() const
{
    return document()->defaultTextOption().flags().testFlag(QTextOption::ShowTabsAndSpaces);
}

void CodeEditor::commentSelection()
{
    SelectedRange(this).skipEmptyLines().modify([this](const QString &line)->QString{
        auto p = splitLine(line);
        return p.indent + _commentSymbol + ' ' + p.text;
    });
}

void CodeEditor::uncommentSelection()
{
    SelectedRange(this).skipEmptyLines().modify([this](const QString &line)->QString{
        auto p = splitLine(line);
        if (p.text.startsWith(_commentSymbol)) {
            p.text = p.text.mid(_commentSymbol.length());
            if (p.text.startsWith(' ')) {
                p.text = p.text.mid(1);
            }
        }
        return p.indent + p.text;
    });
}

void CodeEditor::toggleCommentSelection()
{
    int commentLines = 0, totalLines = 0;
    SelectedRange(this).skipEmptyLines().modify([this, &commentLines, &totalLines](const QString &line)->QString{
        totalLines++;
        if (splitLine(line).text.startsWith(_commentSymbol))
            commentLines++;
        return line;
    });
    if (commentLines > totalLines / 2)
        uncommentSelection();
    else commentSelection();
}

void CodeEditor::indentSelection()
{
    SelectedRange(this).modify([this](const QString &line)->QString{
        return oneIndent() + normalizeIndent(line);
    });
}

void CodeEditor::unindentSelection()
{
    SelectedRange(this).modify([this](const QString &line)->QString{
        return removeOneIndent(normalizeIndent(line));
    });
}

QString CodeEditor::normalizeIndent(const QString& line) const
{
    auto p = splitLine(line);
    QString indent;
    if (_replaceTabs) {
        indent = p.indent.toString();
        indent.replace('\t', tabSpaces());
    } else {
        int spaceCount = 0;
        for (QChar c : std::as_const(p.indent)) {
            if (c == ' ') {
                spaceCount++;
                if (spaceCount == _tabWidth) {
                    indent += '\t';
                    spaceCount = 0;
                }
            } else if (c == '\t') {
                // Add any remaining spaces first
                indent += QString(spaceCount, ' ');
                spaceCount = 0;
                indent += '\t';
            }
        }
        // Add any remaining spaces
        indent += QString(spaceCount, ' ');
    }
    return indent + p.text;
}

QString CodeEditor::removeOneIndent(const QString& line) const
{
    auto p = splitLine(line);
    QStringRef indent;
    if (p.indent.startsWith(' ')) {
        int spaceCount = qMin(_tabWidth, p.indent.length());
        for (int i = 0; i < p.indent.length() && i < spaceCount; i++) {
            indent = p.indent.mid(i+1);
        }
    } else {
        indent = p.indent.mid(1);
    }
    return indent + p.text;
}

void CodeEditor::handleSmartEnter()
{
    auto cursor = textCursor();
    auto line = cursor.block().text();
    auto p = splitLine(line);
    
    // Check if text before cursor ends with block start symbol
    QStringRef textBeforeCursor = line.leftRef(cursor.positionInBlock());
    bool isNewBlock = textBeforeCursor.trimmed().endsWith(_blockStartSymbol);
    
    cursor.beginEditBlock();
    cursor.insertText("\n");
    cursor.insertText(p.indent.toString());
    if (isNewBlock)
        cursor.insertText(oneIndent());
    cursor.endEditBlock();
}

void CodeEditor::handleSmartHome(bool select)
{
    auto cursor = textCursor();
    auto block = cursor.block();
    auto p = splitLine(block.text());
    auto moveMode = select ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor;
    if (cursor.positionInBlock() == p.indent.length())
        cursor.movePosition(QTextCursor::StartOfBlock, moveMode);
    else cursor.setPosition(block.position() + p.indent.length(), moveMode);
    setTextCursor(cursor);
}

void CodeEditor::normalize(NormalizationOptions options)
{
    auto cursor = textCursor();
    auto backupPos = cursor.position();
    cursor.beginEditBlock();
    auto block = document()->firstBlock();
    while (block.isValid()) {
        QString origLine = block.text();
        QString normLine = origLine;
        if (options & NormTrailingSpaces) {
            while (!normLine.isEmpty() && 
                   (normLine.right(1) == " " || normLine.right(1) == "\t")) {
                normLine.chop(1);
            }
        }
        if (options & NormIndentation) {
            if (!isEmpty(normLine))
                normLine = normalizeIndent(normLine);
        }
        if (normLine != origLine) {
            QTextCursor lineCursor(block);
            lineCursor.select(QTextCursor::LineUnderCursor);
            lineCursor.insertText(normLine);
        }
        block = block.next();
    }
    if (options & NormFinalNewline) {
        auto lastBlock = document()->lastBlock();
        if (lastBlock.isValid() && !lastBlock.text().isEmpty()) {
            QTextCursor endCursor(document());
            endCursor.movePosition(QTextCursor::End);
            endCursor.insertText("\n");
        }
    }
    cursor.endEditBlock();
    cursor.setPosition(qMin(backupPos, document()->characterCount() - 1));
    setTextCursor(cursor);
}

void CodeEditor::setFoldingType(FoldingType ft)
{
    if (_foldingType == ft)
        return;
    unfoldAll();
    _foldingType = ft;
    if (ft == FOLD_NONE)
        _codeFolder = {};
    else if (!_codeFolder)
        _codeFolder.reset(new CodeFolder(this));
}

void CodeEditor::fold()
{
    if (_codeFolder)
        _codeFolder->foldCodeBlock(_tabWidth, _foldingType);
}

void CodeEditor::unfold()
{
    if (_codeFolder)
        _codeFolder->unfold();
}

void CodeEditor::unfoldAll()
{
    if (_codeFolder && _codeFolder->hasFoldings())
        setPlainText(_codeFolder->toUnfoldedText());
}

} // namespace Widgets
} // namespace Ori
