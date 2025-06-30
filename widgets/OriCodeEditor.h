#ifndef ORI_CODE_EDITOR_H
#define ORI_CODE_EDITOR_H

#include <QPlainTextEdit>

/**

A simple wrapper around QPlainTextEdit providing several additional features conventional for code editors

- line numbering
- current line highlighting
- line hints for error highlighting

*/

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
    
    void setShowWhitespaces(bool on);
    bool showWhitespaces() const;
    
    bool loadCode(const QString &fileName);
    bool saveCode(const QString &fileName);
    
    // Comment/uncomment functionality
    void setCommentSymbol(const QString& symbol);
    QString commentSymbol() const;
    void commentSelection();
    void uncommentSelection();
    void toggleCommentSelection();
    
    // Tab replacement functionality
    void setReplaceTabsWithSpaces(bool enabled);
    bool replaceTabsWithSpaces() const;
    void setTabSpaceCount(int count);
    int tabSpaceCount() const;
    
    // Indentation functionality
    void indentSelection();
    void unindentSelection();

    struct Style
    {
        QColor currentLineColor;
        QColor lineNumTextColor;
        QColor lineNumTextColorErr;
        QColor lineNumBorderColor;
        QColor lineNumBackColor;
        QColor lineNumBackColorErr;
        int lineNumRightPadding;
        int lineNumLeftPadding;
        int lineNumMargin;
        int lineNumFontSizeDec;
    };
    Style style() const { return _style; }
    void setStyle(const Style &s) { _style = s; }

protected:
    void resizeEvent(QResizeEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    QWidget *_lineNumArea;
    QMap<int, QString> _lineHints;
    Style _style;
    QString _commentSymbol;
    bool _replaceTabsWithSpaces;
    int _tabSpaceCount;
    
    void updateLineNumberAreaWidth(int blockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();
    int findLineNumber(int y) const;
    
    // Comment/uncomment helper methods
    bool isLineCommented(const QString& line) const;
    QString getLineIndentation(const QString& line) const;
    void selectLines(int startLine, int endLine);
    
    // Indentation helper methods
    QString normalizeIndentation(const QString& line) const;
    QString removeOneIndentLevel(const QString& line) const;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_CODE_EDITOR_H
