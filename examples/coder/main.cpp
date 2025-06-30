#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "widgets/OriCodeEditor.h"

#define A_ Ori::Gui::action

class MainWindow : public QMainWindow
{
public:
    MainWindow() : QMainWindow()
    { 
        editor = new Ori::Widgets::CodeEditor;
        editor->setShowWhitespaces(true);
        editor->loadCode("../examples/coder/demo.py");
        Ori::Gui::setFontMonospace(editor);
        Ori::Highlighter::setHighlighter(editor, "../syntax/python.ohl");
        
        auto aShowLineHints = A_("Show line hints", this, [this]{
            editor->setLineHints({{ 13, "Something didn't go" }});
        });
        auto aClearLineHints = A_("Clear line hints", this, [this]{
            editor->setLineHints({});
        });
        
        auto aCommentSelection = A_("Comment Selection", this, [this]{
            editor->commentSelection();
        });
        auto aUncommentSelection = A_("Uncomment Selection", this, [this]{
            editor->uncommentSelection();
        });
        auto aToggleComment = A_("Toggle Comment (Ctrl+/)", this, [this]{
            editor->toggleCommentSelection();
        });
        auto aSetCommentSymbol = A_("Set Comment Symbol to //", this, [this]{
            editor->setCommentSymbol("//");
        });
        auto aResetCommentSymbol = A_("Reset Comment Symbol to #", this, [this]{
            editor->setCommentSymbol("#");
        });
        
        auto aToggleTabReplacement = A_("Toggle Tab Replacement", this, [this]{
            editor->setReplaceTabsWithSpaces(!editor->replaceTabsWithSpaces());
        });
        auto aSetTabSpaces2 = A_("Set Tab Spaces to 2", this, [this]{
            editor->setTabSpaceCount(2);
        });
        auto aSetTabSpaces4 = A_("Set Tab Spaces to 4", this, [this]{
            editor->setTabSpaceCount(4);
        });
        auto aSetTabSpaces8 = A_("Set Tab Spaces to 8", this, [this]{
            editor->setTabSpaceCount(8);
        });
        
        auto aIndentSelection = A_("Indent Selection", this, [this]{
            editor->indentSelection();
        });
        auto aUnindentSelection = A_("Unindent Selection", this, [this]{
            editor->unindentSelection();
        });
        
        auto aToggleAutoIndent = A_("Toggle Auto Indent", this, [this]{
            editor->setAutoIndentEnabled(!editor->autoIndentEnabled());
        });
        auto aSetBlockSymbolColon = A_("Set Block Symbol to :", this, [this]{
            editor->setBlockStartSymbol(":");
        });
        auto aSetBlockSymbolBrace = A_("Set Block Symbol to {", this, [this]{
            editor->setBlockStartSymbol("{");
        });
        
        Ori::Gui::populate(menuBar()->addMenu("View"), {
            aShowLineHints, aClearLineHints
        });
        
        Ori::Gui::populate(menuBar()->addMenu("Edit"), {
            aCommentSelection, aUncommentSelection, aToggleComment, nullptr,
            aSetCommentSymbol, aResetCommentSymbol, nullptr,
            aToggleTabReplacement, aSetTabSpaces2, aSetTabSpaces4, aSetTabSpaces8, nullptr,
            aIndentSelection, aUnindentSelection, nullptr,
            aToggleAutoIndent, aSetBlockSymbolColon, aSetBlockSymbolBrace
        });
        
        setCentralWidget(editor);
        resize(800, 1000);
    }
    
    Ori::Widgets::CodeEditor *editor;
};
    
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
