#include "OriCodeEditor.h"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QToolTip>

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

} // namespace Widgets
} // namespace Ori
