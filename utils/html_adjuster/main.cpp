#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // It is used as settings storage location.
    app.setOrganizationName("orion_examples");

    MainWindow w;
    w.show();

    return app.exec();
}


