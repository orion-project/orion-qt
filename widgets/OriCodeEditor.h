#ifndef ORI_CODE_EDITOR_H
#define ORI_CODE_EDITOR_H

#include <QPlainTextEdit>

/**

A simple wrapper around QPlainTextEdit providing several additional features conventional for code editors

- line numbering
- current line highlighting
- line hints for error highlighting
- tab to space replacement
- selection (un)indentation
- automatic block indentation
- smart home key
- document normalization

*/

namespace Ori {
namespace Widgets {

class FoldedTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit FoldedTextObject(QObject *parent = 0);

    static int type() { return QTextFormat::UserObject+2; }
    static int prop() { return 2; }

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

    void fold(QTextCursor c);
    bool unfold(QTextCursor c);
};

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
    
    // Auto-indentation functionality
    void setBlockStartSymbol(const QString& symbol);
    QString blockStartSymbol() const;
    void setAutoIndentEnabled(bool enabled);
    bool autoIndentEnabled() const;
    
    // Document normalization options
    enum NormalizationOption {
        NormalizeIndentation = 0x01,
        NormalizeTrimTrailingSpaces = 0x02,
        NormalizeEnsureNewlineAtEnd = 0x04,
        NormalizeAll = NormalizeIndentation | NormalizeTrimTrailingSpaces | NormalizeEnsureNewlineAtEnd
    };
    Q_DECLARE_FLAGS(NormalizationOptions, NormalizationOption)
    
    // Document-wide normalization
    void normalizeDocument(NormalizationOptions options = NormalizeAll);

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

    void fold();
    void unfold();
    void foldBlock();
    
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
    QString _blockStartSymbol;
    bool _autoIndentEnabled;
    FoldedTextObject *_textFolder;
    
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
    
    // Auto-indentation helper methods
    void handleAutoIndentOnEnter();
    bool handleAutoUnindentOnBackspace();
    bool isCursorInIndentation() const;
    bool handleSmartHome();
    
    // Folding helper methods for saving
    bool hasFoldedBlocks() const;
    QString getUnfoldedText() const;
    
    // Block detection helper methods
    QPair<int, int> detectPythonBlock(int lineNumber) const;
    int getLineIndentationLevel(const QString& line) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CodeEditor::NormalizationOptions)

} // namespace Widgets
} // namespace Ori

#endif // ORI_CODE_EDITOR_H
