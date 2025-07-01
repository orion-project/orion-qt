#ifndef ORI_CODE_EDITOR_H
#define ORI_CODE_EDITOR_H

#include <QPlainTextEdit>

/**

A wrapper around QPlainTextEdit providing several additional features conventional for code editors

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

class EditorLineNums;
class CodeFolder;

class FoldedTextView : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
public:
    explicit FoldedTextView(QObject *parent) : QObject(parent) {}
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    bool loadCode(const QString &fileName);
    bool saveCode(const QString &fileName);
    void setCode(const QString &code) { setPlainText(code); }
    QString code() const;
    
    QMap<int, QString> lineHints() const { return _lineHints; }
    void setLineHints(const QMap<int, QString>& hints) { _lineHints = hints; update(); }
    
    bool showWhitespaces() const;
    void setShowWhitespaces(bool on);
    
    QString commentSymbol() const { return _commentSymbol; }
    void setCommentSymbol(const QString& s) { _commentSymbol = s; }
    void commentSelection();
    void uncommentSelection();
    void toggleCommentSelection();
    
    bool replaceTabs() const { return _replaceTabs; }
    void setReplaceTabs(bool on) { _replaceTabs = on; }
    int tabWidth() const { return _tabWidth; }
    void setTabWidth(int spaces) { _tabWidth = spaces; }
    
    bool autoIndent() const { return _autoIndent; }
    void setAutoIndent(bool on) { _autoIndent = on; }
    QString blockStartSymbol() const { return _blockStartSymbol; }
    void setBlockStartSymbol(const QString& s) { _blockStartSymbol = s; }
    void indentSelection();
    void unindentSelection();
    
    enum NormalizationOption {
        NormIndentation = 0x01,
        NormTrailingSpaces = 0x02,
        NormFinalNewline = 0x04,
        NormAll = NormIndentation | NormTrailingSpaces | NormFinalNewline
    };
    Q_DECLARE_FLAGS(NormalizationOptions, NormalizationOption)
    
    void normalize(NormalizationOptions options = NormAll);

    struct Style
    {
        QColor currentLineColor;
        QColor lineNumsTextColor;
        QColor lineNumsTextColorErr;
        QColor lineNumsBorderColor;
        QColor lineNumsBackColor;
        QColor lineNumsBackColorErr;
        int lineNumsRightPadding;
        int lineNumsLeftPadding;
        int lineNumsMargin;
        int lineNumsFontSizeDec;
    };
    Style style() const { return _style; }
    void setStyle(const Style &s) { _style = s; }

    enum FoldingType { FOLD_NONE, FOLD_PYTHON };
    FoldingType foldingType() const { return _foldingType; }
    void setFoldingType(FoldingType f);
    void fold();
    void unfold();
    void unfoldAll();
    
protected:
    void resizeEvent(QResizeEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    class EditorLineNums *_lineNums;
    QMap<int, QString> _lineHints;
    Style _style;
    bool _replaceTabs = true;
    bool _autoIndent = true;
    int _tabWidth = 4;
    QString _commentSymbol = "#";
    QString _blockStartSymbol = ":";
    FoldingType _foldingType = FOLD_NONE;
    std::shared_ptr<CodeFolder> _codeFolder;
    
    void onBlockCountChanged();
    void onDocUpdateRequest(const QRect &rect, int dy);

    void highlightCurrentLine();
    
    QString normalizeIndent(const QString& line) const;
    QString removeOneIndent(const QString& line) const;
    QString tabSpaces() const { return QString(_tabWidth, ' '); }
    QString oneIndent() const { return _replaceTabs ? tabSpaces() : "\t"; }
    
    void handleSmartEnter();
    void handleSmartHome();
    
    friend class EditorLineNums;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CodeEditor::NormalizationOptions)

} // namespace Widgets
} // namespace Ori

#endif // ORI_CODE_EDITOR_H
