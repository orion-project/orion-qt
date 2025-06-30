#include "OriCodeEditor.h"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QToolTip>
#include <QKeyEvent>
#include <QTextCursor>

namespace {

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(Ori::Widgets::CodeEditor *editor) : QWidget(editor), _editor(editor)
    {}

    QSize sizeHint() const override {
        return QSize(_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        _editor->lineNumberAreaPaintEvent(event);
    }

    bool event(QEvent *event) override
    {
        if (event->type() != QEvent::ToolTip)
            return QWidget::event(event);

        auto helpEvent = dynamic_cast<QHelpEvent*>(event);
        if (!helpEvent) return false;

        auto hint = _editor->getLineHint(helpEvent->pos().y());
        if (hint.isEmpty())
            QToolTip::hideText();
        else
            QToolTip::showText(helpEvent->globalPos(), hint);
        event->accept();
        return true;
    }

private:
    Ori::Widgets::CodeEditor *_editor;
};

} // namespace

namespace Ori {
namespace Widgets {

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    setWordWrapMode(QTextOption::NoWrap);

    _lineNumArea = new LineNumberArea(this);
    _commentSymbol = "#"; // default comment symbol for Python
    _replaceTabsWithSpaces = true; // enabled by default
    _tabSpaceCount = 4; // 4 spaces by default
    _blockStartSymbol = ":"; // default block start symbol for Python
    _autoIndentEnabled = true; // enabled by default

    // default style
    _style.currentLineColor = QColor("steelBlue").lighter(220);
    _style.lineNumTextColor = Qt::gray;
    _style.lineNumTextColorErr = Qt::white;
    _style.lineNumBorderColor = QColor(0xfff0f0f0);
    _style.lineNumBackColor = QColor(0xfffafafa);
    _style.lineNumBackColorErr = QColor(Qt::red).lighter(130);
    _style.lineNumRightPadding = 8;
    _style.lineNumLeftPadding = 12;
    _style.lineNumMargin = 4;
    _style.lineNumFontSizeDec = 2;

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    
    setTabStopDistance(40); // twice less that the default
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    _lineNumArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    // in a plain text edit each line will consist of one QTextBlock
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        digits++;
    }
    return _style.lineNumLeftPadding + _style.lineNumRightPadding + _style.lineNumMargin +
            fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLineNumberAreaWidth(int blockCount)
{
    Q_UNUSED(blockCount)
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        _lineNumArea->scroll(0, dy);
    else
        _lineNumArea->update(0, rect.y(), _lineNumArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
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

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    auto r = event->rect();
    int lineNumW = r.width();
    int borderPos = lineNumW - _style.lineNumMargin;

    QPainter painter(_lineNumArea);
    if (_style.lineNumMargin > 0)
        painter.fillRect(r, Qt::white);
    r.setWidth(r.width() - _style.lineNumMargin);
    painter.fillRect(r, _style.lineNumBackColor);

    painter.setPen(_style.lineNumBorderColor);
    painter.drawLine(borderPos, r.top(), borderPos, r.bottom());

    if (_style.lineNumFontSizeDec > 0) {
        auto f = painter.font();
        f.setPointSize(f.pointSize()-_style.lineNumFontSizeDec);
        painter.setFont(f);
    }

    QTextBlock block = firstVisibleBlock();
    int lineNum = block.blockNumber() + 1;
    int lineTop = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int lineH = qRound(blockBoundingRect(block).height());
    int lineBot = lineTop + lineH;
    while (block.isValid() && lineTop <= r.bottom())
    {
        if (block.isVisible() && lineBot >= r.top())
        {
            if (_lineHints.contains(lineNum))
            {
                painter.setPen(_style.lineNumTextColorErr);
                painter.fillRect(0, lineTop, lineNumW - _style.lineNumMargin, lineH, _style.lineNumBackColorErr);
            }
            else
                painter.setPen(_style.lineNumTextColor);
            painter.drawText(0, lineTop, lineNumW - _style.lineNumRightPadding - _style.lineNumMargin, lineH,
                Qt::AlignRight|Qt::AlignVCenter, QString::number(lineNum));
        }
        block = block.next();
        lineTop = lineBot;
        lineH = qRound(blockBoundingRect(block).height());
        lineBot = lineTop + lineH;
        lineNum++;
    }
}

void CodeEditor::setLineHints(const QMap<int, QString>& hints)
{
    _lineHints = hints;
    update();
}

int CodeEditor::findLineNumber(int y) const
{
    QTextBlock block = firstVisibleBlock();
    int lineNum = block.blockNumber() + 1;
    int lineTop = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int lineBot = lineTop + qRound(blockBoundingRect(block).height());
    while (block.isValid())
    {
        if (y >= lineTop && y < lineBot) {
            return lineNum;
        }
        block = block.next();
        lineTop = lineBot;
        lineBot = lineTop + qRound(blockBoundingRect(block).height());
        lineNum++;
    }
    return 0;
}

QString CodeEditor::getLineHint(int y) const
{
    return _lineHints.isEmpty() ? QString() : _lineHints.value(findLineNumber(y));
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

bool CodeEditor::loadCode(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning() << "Failed to open" << fileName << f.errorString();
        return false;
    }
    setPlainText(QString::fromUtf8(f.readAll()));
    return true;
}

bool CodeEditor::saveCode(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
        qWarning() << "Failed to open" << fileName << f.errorString();
        return false;
    }
    f.write(toPlainText().toUtf8());
    return true;
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    // Handle Ctrl+/ for toggle comment
    if (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Slash) {
        toggleCommentSelection();
        return;
    }
    
    // Handle auto-indentation on Enter
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        if (_autoIndentEnabled) {
            handleAutoIndentOnEnter();
            return;
        }
    }
    
    // Handle auto-unindent on Backspace
    if (e->key() == Qt::Key_Backspace) {
        if (_autoIndentEnabled && handleAutoUnindentOnBackspace()) {
            return;
        }
    }
    
    // Handle Tab and Shift+Tab for indentation when text is selected
    if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection()) {
            if (e->key() == Qt::Key_Backtab || e->modifiers() == Qt::ShiftModifier) {
                unindentSelection();
            } else {
                indentSelection();
            }
            return;
        } else if (e->key() == Qt::Key_Tab && _replaceTabsWithSpaces) {
            // Handle Tab replacement with spaces for single cursor
            QString spaces(static_cast<int>(_tabSpaceCount), ' ');
            insertPlainText(spaces);
            return;
        }
    }
    
    QPlainTextEdit::keyPressEvent(e);
}

void CodeEditor::setCommentSymbol(const QString& symbol)
{
    _commentSymbol = symbol;
}

QString CodeEditor::commentSymbol() const
{
    return _commentSymbol;
}

void CodeEditor::commentSelection()
{
    QTextCursor cursor = textCursor();
    
    // If no selection, work with current line
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::LineUnderCursor);
    }
    
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    
    // Get start and end blocks
    QTextCursor startCursor(document());
    startCursor.setPosition(startPos);
    QTextCursor endCursor(document());
    endCursor.setPosition(endPos);
    
    int startBlock = startCursor.blockNumber();
    int endBlock = endCursor.blockNumber();
    
    // If selection ends at the beginning of a line, don't include that line
    if (endCursor.atBlockStart() && endPos > startPos) {
        endBlock--;
    }
    
    cursor.beginEditBlock();
    
    // Comment each line
    for (int blockNum = startBlock; blockNum <= endBlock; blockNum++) {
        QTextBlock block = document()->findBlockByNumber(blockNum);
        if (!block.isValid()) continue;
        
        QString line = block.text();
        if (line.trimmed().isEmpty()) continue; // Skip empty lines
        
        QString indentation = getLineIndentation(line);
        QString commentedLine = indentation + _commentSymbol + " " + line.mid(indentation.length());
        
        QTextCursor lineCursor(block);
        lineCursor.select(QTextCursor::LineUnderCursor);
        lineCursor.insertText(commentedLine);
    }
    
    cursor.endEditBlock();
    
    // Restore selection
    selectLines(startBlock, endBlock);
}

void CodeEditor::uncommentSelection()
{
    QTextCursor cursor = textCursor();
    
    // If no selection, work with current line
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::LineUnderCursor);
    }
    
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    
    // Get start and end blocks
    QTextCursor startCursor(document());
    startCursor.setPosition(startPos);
    QTextCursor endCursor(document());
    endCursor.setPosition(endPos);
    
    int startBlock = startCursor.blockNumber();
    int endBlock = endCursor.blockNumber();
    
    // If selection ends at the beginning of a line, don't include that line
    if (endCursor.atBlockStart() && endPos > startPos) {
        endBlock--;
    }
    
    cursor.beginEditBlock();
    
    // Uncomment each line
    for (int blockNum = startBlock; blockNum <= endBlock; blockNum++) {
        QTextBlock block = document()->findBlockByNumber(blockNum);
        if (!block.isValid()) continue;
        
        QString line = block.text();
        if (!isLineCommented(line)) continue; // Skip non-commented lines
        
        QString indentation = getLineIndentation(line);
        QString trimmedLine = line.mid(indentation.length());
        
        // Remove comment symbol and optional following space
        if (trimmedLine.startsWith(_commentSymbol)) {
            trimmedLine = trimmedLine.mid(_commentSymbol.length());
            if (trimmedLine.startsWith(" ")) {
                trimmedLine = trimmedLine.mid(1);
            }
        }
        
        QString uncommentedLine = indentation + trimmedLine;
        
        QTextCursor lineCursor(block);
        lineCursor.select(QTextCursor::LineUnderCursor);
        lineCursor.insertText(uncommentedLine);
    }
    
    cursor.endEditBlock();
    
    // Restore selection
    selectLines(startBlock, endBlock);
}

void CodeEditor::toggleCommentSelection()
{
    QTextCursor cursor = textCursor();
    
    // If no selection, work with current line
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::LineUnderCursor);
    }
    
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    
    // Get start and end blocks
    QTextCursor startCursor(document());
    startCursor.setPosition(startPos);
    QTextCursor endCursor(document());
    endCursor.setPosition(endPos);
    
    int startBlock = startCursor.blockNumber();
    int endBlock = endCursor.blockNumber();
    
    // If selection ends at the beginning of a line, don't include that line
    if (endCursor.atBlockStart() && endPos > startPos) {
        endBlock--;
    }
    
    // Check if majority of lines are commented
    int commentedLines = 0;
    int totalLines = 0;
    
    for (int blockNum = startBlock; blockNum <= endBlock; blockNum++) {
        QTextBlock block = document()->findBlockByNumber(blockNum);
        if (!block.isValid()) continue;
        
        QString line = block.text();
        if (line.trimmed().isEmpty()) continue; // Skip empty lines
        
        totalLines++;
        if (isLineCommented(line)) {
            commentedLines++;
        }
    }
    
    // If majority are commented, uncomment; otherwise comment
    if (commentedLines > totalLines / 2) {
        uncommentSelection();
    } else {
        commentSelection();
    }
}

bool CodeEditor::isLineCommented(const QString& line) const
{
    QString trimmedLine = line.trimmed();
    return trimmedLine.startsWith(_commentSymbol);
}

QString CodeEditor::getLineIndentation(const QString& line) const
{
    int i = 0;
    while (i < line.length() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }
    return line.left(i);
}

void CodeEditor::selectLines(int startLine, int endLine)
{
    QTextBlock startBlock = document()->findBlockByNumber(startLine);
    QTextBlock endBlock = document()->findBlockByNumber(endLine);
    
    if (!startBlock.isValid() || !endBlock.isValid()) return;
    
    QTextCursor cursor(startBlock);
    cursor.movePosition(QTextCursor::StartOfBlock);
    
    // Set the anchor at the start of the first line
    int startPos = cursor.position();
    
    // Move to the end of the last line
    cursor.setPosition(endBlock.position() + endBlock.length() - 1);
    
    // Create selection from start to end
    cursor.setPosition(startPos, QTextCursor::KeepAnchor);
    
    setTextCursor(cursor);
}

void CodeEditor::setReplaceTabsWithSpaces(bool enabled)
{
    _replaceTabsWithSpaces = enabled;
}

bool CodeEditor::replaceTabsWithSpaces() const
{
    return _replaceTabsWithSpaces;
}

void CodeEditor::setTabSpaceCount(int count)
{
    _tabSpaceCount = count;
}

int CodeEditor::tabSpaceCount() const
{
    return _tabSpaceCount;
}

void CodeEditor::indentSelection()
{
    QTextCursor cursor = textCursor();
    
    // If no selection, work with current line
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::LineUnderCursor);
    }
    
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    
    // Get start and end blocks
    QTextCursor startCursor(document());
    startCursor.setPosition(startPos);
    QTextCursor endCursor(document());
    endCursor.setPosition(endPos);
    
    int startBlock = startCursor.blockNumber();
    int endBlock = endCursor.blockNumber();
    
    // If selection ends at the beginning of a line, don't include that line
    if (endCursor.atBlockStart() && endPos > startPos) {
        endBlock--;
    }
    
    cursor.beginEditBlock();
    
    // Determine indentation string based on settings
    QString indentString = _replaceTabsWithSpaces ? QString(_tabSpaceCount, ' ') : "\t";
    
    // Indent each line
    for (int blockNum = startBlock; blockNum <= endBlock; blockNum++) {
        QTextBlock block = document()->findBlockByNumber(blockNum);
        if (!block.isValid()) continue;
        
        QString line = block.text();
        if (line.trimmed().isEmpty()) continue; // Skip empty lines
        
        // Normalize existing indentation if needed
        QString normalizedLine = normalizeIndentation(line);
        QString indentedLine = indentString + normalizedLine;
        
        QTextCursor lineCursor(block);
        lineCursor.select(QTextCursor::LineUnderCursor);
        lineCursor.insertText(indentedLine);
    }
    
    cursor.endEditBlock();
    
    // Restore selection
    selectLines(startBlock, endBlock);
}

void CodeEditor::unindentSelection()
{
    QTextCursor cursor = textCursor();
    
    // If no selection, work with current line
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::LineUnderCursor);
    }
    
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    
    // Get start and end blocks
    QTextCursor startCursor(document());
    startCursor.setPosition(startPos);
    QTextCursor endCursor(document());
    endCursor.setPosition(endPos);
    
    int startBlock = startCursor.blockNumber();
    int endBlock = endCursor.blockNumber();
    
    // If selection ends at the beginning of a line, don't include that line
    if (endCursor.atBlockStart() && endPos > startPos) {
        endBlock--;
    }
    
    cursor.beginEditBlock();
    
    // Unindent each line
    for (int blockNum = startBlock; blockNum <= endBlock; blockNum++) {
        QTextBlock block = document()->findBlockByNumber(blockNum);
        if (!block.isValid()) continue;
        
        QString line = block.text();
        if (line.trimmed().isEmpty()) continue; // Skip empty lines
        
        QString unindentedLine = removeOneIndentLevel(line);
        // Normalize the remaining indentation to match current settings
        unindentedLine = normalizeIndentation(unindentedLine);
        
        QTextCursor lineCursor(block);
        lineCursor.select(QTextCursor::LineUnderCursor);
        lineCursor.insertText(unindentedLine);
    }
    
    cursor.endEditBlock();
    
    // Restore selection
    selectLines(startBlock, endBlock);
}

QString CodeEditor::normalizeIndentation(const QString& line) const
{
    QString indentation = getLineIndentation(line);
    QString content = line.mid(indentation.length());
    
    if (_replaceTabsWithSpaces) {
        // Convert tabs to spaces
        QString normalizedIndent = indentation;
        normalizedIndent.replace('\t', QString(_tabSpaceCount, ' '));
        return normalizedIndent + content;
    } else {
        // Convert spaces to tabs (groups of _tabSpaceCount spaces become one tab)
        QString normalizedIndent;
        int spaceCount = 0;
        for (QChar c : indentation) {
            if (c == ' ') {
                spaceCount++;
                if (spaceCount == _tabSpaceCount) {
                    normalizedIndent += '\t';
                    spaceCount = 0;
                }
            } else if (c == '\t') {
                // Add any remaining spaces first
                normalizedIndent += QString(spaceCount, ' ');
                spaceCount = 0;
                normalizedIndent += '\t';
            }
        }
        // Add any remaining spaces
        normalizedIndent += QString(spaceCount, ' ');
        return normalizedIndent + content;
    }
}

QString CodeEditor::removeOneIndentLevel(const QString& line) const
{
    if (line.isEmpty()) return line;
    
    QString indentation = getLineIndentation(line);
    QString content = line.mid(indentation.length());
    
    if (indentation.isEmpty()) return line;
    
    QString newIndentation;
    
    if (_replaceTabsWithSpaces) {
        // Remove up to _tabSpaceCount spaces or one tab
        if (indentation.startsWith('\t')) {
            newIndentation = indentation.mid(1);
        } else {
            int spacesToRemove = qMin(_tabSpaceCount, indentation.length());
            int removed = 0;
            for (int i = 0; i < indentation.length() && removed < spacesToRemove; i++) {
                if (indentation[i] == ' ') {
                    removed++;
                } else {
                    newIndentation += indentation.mid(i);
                    break;
                }
            }
            if (removed == spacesToRemove && removed < indentation.length()) {
                newIndentation += indentation.mid(removed);
            }
        }
    } else {
        // Remove one tab or up to _tabSpaceCount spaces
        if (indentation.startsWith('\t')) {
            newIndentation = indentation.mid(1);
        } else {
            int spacesToRemove = qMin(_tabSpaceCount, indentation.length());
            int removed = 0;
            for (int i = 0; i < indentation.length() && removed < spacesToRemove; i++) {
                if (indentation[i] == ' ') {
                    removed++;
                } else {
                    newIndentation += indentation.mid(i);
                    break;
                }
            }
            if (removed == spacesToRemove && removed < indentation.length()) {
                newIndentation += indentation.mid(removed);
            }
        }
    }
    
    return newIndentation + content;
}

void CodeEditor::setBlockStartSymbol(const QString& symbol)
{
    _blockStartSymbol = symbol;
}

QString CodeEditor::blockStartSymbol() const
{
    return _blockStartSymbol;
}

void CodeEditor::setAutoIndentEnabled(bool enabled)
{
    _autoIndentEnabled = enabled;
}

bool CodeEditor::autoIndentEnabled() const
{
    return _autoIndentEnabled;
}

void CodeEditor::handleAutoIndentOnEnter()
{
    QTextCursor cursor = textCursor();
    QTextBlock currentBlock = cursor.block();
    QString currentLine = currentBlock.text();
    
    // Get indentation of current line
    QString indentation = getLineIndentation(currentLine);
    
    // Check if current line ends with block start symbol
    QString trimmedLine = currentLine.trimmed();
    bool shouldIndentMore = trimmedLine.endsWith(_blockStartSymbol);
    
    // Insert new line
    cursor.insertText("\n");
    
    // Add base indentation
    cursor.insertText(indentation);
    
    // Add extra indentation if needed
    if (shouldIndentMore) {
        QString extraIndent = _replaceTabsWithSpaces ? QString(_tabSpaceCount, ' ') : "\t";
        cursor.insertText(extraIndent);
    }
}

bool CodeEditor::handleAutoUnindentOnBackspace()
{
    QTextCursor cursor = textCursor();
    
    // Only handle if cursor is in indentation area
    if (!isCursorInIndentation()) {
        return false; // Let default backspace handle it
    }
    
    QTextBlock currentBlock = cursor.block();
    QString currentLine = currentBlock.text();
    QString indentation = getLineIndentation(currentLine);
    
    if (indentation.isEmpty()) {
        return false; // No indentation to remove
    }
    
    // Remove one indentation level
    QString newIndentation = removeOneIndentLevel(currentLine);
    newIndentation = normalizeIndentation(newIndentation);
    
    // Replace the entire line
    QTextCursor lineCursor(currentBlock);
    lineCursor.select(QTextCursor::LineUnderCursor);
    lineCursor.insertText(newIndentation);
    
    // Position cursor at end of new indentation
    cursor.setPosition(currentBlock.position() + getLineIndentation(newIndentation).length());
    setTextCursor(cursor);
    
    return true; // We handled the backspace
}

bool CodeEditor::isCursorInIndentation() const
{
    QTextCursor cursor = textCursor();
    QTextBlock currentBlock = cursor.block();
    QString currentLine = currentBlock.text();
    
    int cursorPosInLine = cursor.positionInBlock();
    QString indentation = getLineIndentation(currentLine);
    
    return cursorPosInLine <= indentation.length();
}

} // namespace Widgets
} // namespace Ori
