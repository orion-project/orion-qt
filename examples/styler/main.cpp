#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include "../../tools/OriStyler.h"
#include "../../tools/OriSettings.h"
#include "../../widgets/OriStylesMenu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("fusion");

    // Styler object can enumtrate and apply available application styles.
    Ori::Styler styler;

    // Restore previously selected style.
    Ori::Settings settings;
    styler.setCurrentStyle(settings.strValue("style"));

    QMainWindow w;
    // StylesMenu can display all available styles and select them through Styler.
    w.menuBar()->addMenu(new Ori::Widgets::StylesMenu(&styler, &w));
    w.show();

    int res = a.exec();

    // Store selected style.
    settings.setValue("style", styler.currentStyle());

    return res;
}
