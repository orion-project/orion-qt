#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // It is used as settings storage location.
    a.setOrganizationName("orion_examples");

    MainWindow w;
    w.show();

    return a.exec();
}
