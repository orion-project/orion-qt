#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("orion-project.org");
    app.setStyle("fusion");

    MainWindow w;
    w.show();

    return app.exec();
}


