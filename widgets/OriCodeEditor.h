#ifndef ORI_CODE_EDITOR_H
#define ORI_CODE_EDITOR_H

#include <QPlainTextEdit>

namespace Ori {
namespace Widgets {

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    void setLineHints(const QMap<int, QString>& hints);
    QString getLineHint(int y) const;

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    QWidget *_lineNumArea;
    QMap<int, QString> _lineHints;
    void updateLineNumberAreaWidth(int blockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();
    int findLineNumber(int y) const;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_CODE_EDITOR_H
