#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "widgets/OriCodeEditor.h"

#define A_ Ori::Gui::action
#define C_ Ori::Gui::checkableAction
#define ________ nullptr,

class MainWindow : public QMainWindow
{
public:
    MainWindow() : QMainWindow()
    { 
        editor = new Ori::Widgets::CodeEditor;
        editor->setShowWhitespaces(true);
        editor->loadCode(fileName);
        Ori::Gui::setFontMonospace(editor);
        Ori::Highlighter::setHighlighter(editor, "../syntax/python.ohl");
        
        Ori::Gui::populate(menuBar()->addMenu("File"), {
            A_("Reload File", this, [this]{ editor->loadCode(fileName); }),
            A_("Save File", this, [this]{ editor->saveCode(fileName); }),
        });
        
        Ori::Gui::populate(menuBar()->addMenu("View"), {
            A_("Show line hints", this, [this]{ editor->setLineHints({
                { 13, "Something didn't go here" },
                { 23, "Something wnt wrong here" },
            }); }),
            A_("Clear line hints", this, [this]{ editor->setLineHints({}); }),
            ________
            C_("Show Whitespaces", editor->showWhitespaces(), this, [this]{
                editor->setShowWhitespaces(!editor->showWhitespaces());
            }),
            ________
            A_("Fold Selection", this, [this]{ editor->foldSelection(); }),
            A_("Fold Block", this, [this]{ editor->foldCodeBlock(); }),
            A_("Unfold", this, [this]{ editor->unfold(); }),
            A_("Unfold All", this, [this]{ editor->unfoldAll(); }),
        });
        
        Ori::Gui::populate(menuBar()->addMenu("Edit"), {
            A_("Comment Selection", this, [this]{ editor->commentSelection(); }),
            A_("Uncomment Selection", this, [this]{ editor->uncommentSelection(); }),
            A_("Toggle Comment (Ctrl+/)", this, [this]{ editor->toggleCommentSelection(); }),
            ________
            A_("Set Comment Symbol to //", this, [this]{ editor->setCommentSymbol("//"); }), 
            A_("Set Comment Symbol to #", this, [this]{ editor->setCommentSymbol("#"); }),
            ________
            C_("Replace Tabs With Spaces", editor->replaceTabs(), this, [this]{
              editor->setReplaceTabs(!editor->replaceTabs());
            }), 
            A_("Set Tab Spaces to 2", this, [this]{ editor->setTabWidth(2); }), 
            A_("Set Tab Spaces to 4", this, [this]{ editor->setTabWidth(4); }),
            A_("Set Tab Spaces to 8", this, [this]{ editor->setTabWidth(8); }),
            ________
            A_("Indent Selection", this, [this]{ editor->indentSelection(); }),
            A_("Unindent Selection", this, [this]{ editor->unindentSelection(); }),
            ________
            C_("Auto Indent", editor->autoIndent(), this, [this]{
              editor->setAutoIndent(!editor->autoIndent());
            }),
            A_("Set Block Symbol to :", this, [this]{ editor->setBlockStartSymbol(":"); }),
            A_("Set Block Symbol to {", this, [this]{ editor->setBlockStartSymbol("{"); }),
            ________
            A_("Normalize Document (All)", this, [this]{ editor->normalize(); }),
            A_("Normalize Document (Indentation)", this, [this]{
              editor->normalize(Ori::Widgets::CodeEditor::NormIndentation);
            }),
            A_("Normalize Document (Trailing Spaces)", this, [this]{
                editor->normalize(Ori::Widgets::CodeEditor::NormTrailingSpaces);
            }),
            A_("Normalize Document (Newline at End)", this, [this]{
                editor->normalize(Ori::Widgets::CodeEditor::NormFinalNewline);
            }),
        });
     
        statusBar()->addWidget(new QLabel(fileName)); 
        setCentralWidget(editor);
        resize(800, 1000);
    }
    
    Ori::Widgets::CodeEditor *editor;
    QString fileName = "../examples/coder/demo.py";
};
    
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
