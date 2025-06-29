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
        
        Ori::Gui::populate(menuBar()->addMenu("View"), {
            aShowLineHints, aClearLineHints
        });
        
        Ori::Gui::populate(menuBar()->addMenu("Edit"), {
            aCommentSelection, aUncommentSelection, aToggleComment, nullptr,
            aSetCommentSymbol, aResetCommentSymbol
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
