#include "AppConfig.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>

class MainWindow : public QMainWindow
{
public:
    MainWindow() : QMainWindow()
    {
        auto tb = new QToolBar;

        tb->addAction("Edit Config...", []{
            AppConfig::instance().edit();
        });

        tb->addAction("Edit Config (activate second page)...", []{
            AppConfig::instance().edit(AppConfig::pageInterface);
        });

        addToolBar(tb);
    }
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("fusion");

    // Load application settings before any command start
    AppConfig::instance().load();

    MainWindow w;
    w.show();
    return a.exec();
}
