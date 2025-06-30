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
            A_("Fold", this, [this]{ editor->fold(); }),
            A_("Unfold", this, [this]{ editor->unfold(); }),
        });
        
        Ori::Gui::populate(menuBar()->addMenu("Edit"), {
            A_("Comment Selection", this, [this]{ editor->commentSelection(); }),
            A_("Uncomment Selection", this, [this]{ editor->uncommentSelection(); }),
            A_("Toggle Comment (Ctrl+/)", this, [this]{ editor->toggleCommentSelection(); }),
            ________
            A_("Set Comment Symbol to //", this, [this]{ editor->setCommentSymbol("//"); }), 
            A_("Set Comment Symbol to #", this, [this]{ editor->setCommentSymbol("#"); }),
            ________
            C_("Replace Tabs With Spaces", editor->replaceTabsWithSpaces(), this, [this]{
              editor->setReplaceTabsWithSpaces(!editor->replaceTabsWithSpaces());
            }), 
            A_("Set Tab Spaces to 2", this, [this]{ editor->setTabSpaceCount(2); }), 
            A_("Set Tab Spaces to 4", this, [this]{ editor->setTabSpaceCount(4); }),
            A_("Set Tab Spaces to 8", this, [this]{ editor->setTabSpaceCount(8); }),
            ________
            A_("Indent Selection", this, [this]{ editor->indentSelection(); }),
            A_("Unindent Selection", this, [this]{ editor->unindentSelection(); }),
            ________
            C_("Auto Indent", editor->autoIndentEnabled(), this, [this]{
              editor->setAutoIndentEnabled(!editor->autoIndentEnabled());
            }),
            A_("Set Block Symbol to :", this, [this]{ editor->setBlockStartSymbol(":"); }),
            A_("Set Block Symbol to {", this, [this]{ editor->setBlockStartSymbol("{"); }),
            ________
            A_("Normalize Document (All)", this, [this]{ editor->normalizeDocument(); }),
            A_("Normalize Document (Indentation)", this, [this]{
              editor->normalizeDocument(Ori::Widgets::CodeEditor::NormalizeIndentation);
            }),
            A_("Normalize Document (Trailing Spaces)", this, [this]{
                editor->normalizeDocument(Ori::Widgets::CodeEditor::NormalizeTrimTrailingSpaces);
            }),
            A_("Normalize Document (Newline at End)", this, [this]{
                editor->normalizeDocument(Ori::Widgets::CodeEditor::NormalizeEnsureNewlineAtEnd);
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
