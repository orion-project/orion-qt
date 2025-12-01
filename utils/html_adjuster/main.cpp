#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("fusion");

    MainWindow w;
    w.show();

    return app.exec();
}


