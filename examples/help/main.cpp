#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "../../tools/OriHelpWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("fusion");

    // Show dev commands for style editing
    Ori::HelpWindow::isDevMode = true;
    Ori::HelpWindow::getCssSrc = []{
        return QString(qApp->applicationDirPath() + "/../examples/help/help.css");
    };
    Ori::HelpWindow::getHelpDir = []{
        return QString(qApp->applicationDirPath() + "/../examples/help/help");
    };

    QMainWindow w;

    auto m = w.menuBar()->addMenu("Help");
    m->addAction(QIcon(":/ori_images/help"), "Content", &w, []{ Ori::HelpWindow::showContent(); });
    m->addAction(QIcon(":/ori_images/book"), "Topic", &w, []{ Ori::HelpWindow::showTopic("dev"); });

    w.show();

    return a.exec();
}
