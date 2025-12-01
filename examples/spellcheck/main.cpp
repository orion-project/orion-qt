#include <QApplication>
#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QPlainTextEdit>

#include "helpers/OriLayouts.h"
#include "tools/OriSettings.h"
#include "tools/OriSpellcheck.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow() : QMainWindow()
    { 
        editor1 = new QTextEdit;
        editor1->setStyleSheet("QTextEdit{font-size:16px;border:none}");
        spellcheck1 = new Ori::Spellcheck(editor1);
        
        editor2 = new QPlainTextEdit;
        editor2->setStyleSheet("QPlainTextEdit{font-size:16px;border:none}");
        spellcheck2 = new Ori::Spellcheck(editor2);
        
        tabs = new QTabWidget;
        tabs->addTab(editor1, "QTextEdit");
        tabs->addTab(editor2, "QPlainTextEdit");
        
        Ori::Settings s;
        editor1->setHtml(QString::fromUtf8(QByteArray::fromBase64(s.value("editor1/text").toByteArray())));
        editor2->setPlainText(QString::fromUtf8(QByteArray::fromBase64(s.value("editor2/text").toByteArray())));
        spellcheck1->setLang(s.value("editor1/lang").toString());
        spellcheck2->setLang(s.value("editor2/lang").toString());
        
        spellcheckControl = new Ori::SpellcheckControl(this);
        connect(spellcheckControl, &Ori::SpellcheckControl::langSelected, this, [this](const QString &lang){
            spellcheck()->setLang(lang);
        });

        auto spellcheckMenu = spellcheckControl->makeMenu(this);
        connect(spellcheckMenu, &QMenu::aboutToShow, this, [this]{
            spellcheckControl->showCurrentLang(spellcheck()->lang());
        });
        menuBar()->addMenu(spellcheckMenu);
        
        setCentralWidget(Ori::Layouts::LayoutV({tabs}).setMargin(6).makeWidget());
        resize(800, 600);
    }
    
    ~MainWindow()
    {
        Ori::Settings s;
        s.setValue("editor1/lang", spellcheck1->lang());
        s.setValue("editor2/lang", spellcheck2->lang());
        s.setValue("editor1/text", editor1->toHtml().toUtf8().toBase64());
        s.setValue("editor2/text", editor2->toPlainText().toUtf8().toBase64());
    }
    
    Ori::Spellcheck* spellcheck()
    {
        return tabs->currentIndex() == 0 ? spellcheck1 : spellcheck2;
    }
    
    QTextEdit *editor1;
    QPlainTextEdit *editor2;
    Ori::Spellcheck *spellcheck1;
    Ori::Spellcheck *spellcheck2;
    Ori::SpellcheckControl *spellcheckControl;
    QTabWidget *tabs;
};
    
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("fusion");
    
    MainWindow w;
    w.show();
    return a.exec();
}
