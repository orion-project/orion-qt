#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>

#include "../../helpers/OriTools.h"
#include "../../helpers/OriLayouts.h"
#include "../../widgets/OriColorSelectors.h"
#include "../../widgets/OriStylesMenu.h"

using namespace Ori::Layouts;
using namespace Ori::Widgets;

class MainWindow : public QMainWindow
{
public:
    MainWindow() : QMainWindow()
    {
        menuBar()->addMenu(new Ori::Widgets::StylesMenu(this));

        QList<QColor> colors;
        for (int i = 0; i < 95; i++)
            colors << Ori::Color::random();

        auto edit = new QLineEdit;

        auto colorGrid = new Ori::Widgets::ColorGrid;
        colorGrid->setShowTooltip(true);
        colorGrid->setColors(colors);
        connect(colorGrid, &ColorGrid::hovered, this, [](const QColor& c){
            qDebug() << "Hovered" << c;
        });
        connect(colorGrid, &ColorGrid::selected, this, [edit](const QColor& c){
            qDebug() << "Selected" << c;
            edit->setText(c.name());
        });

        auto colorButton = new Ori::Widgets::ColorButton;
        colorButton->setIconSize({24, 24});

        auto colorButton1 = new Ori::Widgets::ColorButton;
        colorButton1->setIconSize({24, 24});
        colorButton1->setIconRect(QRect(2, 2, 20, 20));

        auto colorButton2 = new Ori::Widgets::ColorButton;
        colorButton2->setIconSize({24, 24});
        colorButton2->setBaseIcon(QIcon(":/toolbar/back_color"));
        colorButton2->setIconRect(QRect(0, 18, 23, 5));

        auto popupColorButton = new Ori::Widgets::PopupColorButton;
        popupColorButton->setColors(colors);
        popupColorButton->setIconSize({24, 24});
        popupColorButton->setShowColorDialog(false);

        auto popupColorButton1 = new Ori::Widgets::PopupColorButton;
        popupColorButton1->setBaseIcon(QIcon(":/toolbar/text_color"));
        popupColorButton1->setIconRect(QRect(0, 18, 23, 5));
        popupColorButton1->setColors(colors);
        popupColorButton1->setIconSize({24, 24});

        setCentralWidget(
            LayoutH({
                LayoutV({
                    new QLabel("<b>ColorGrid</b>"),
                    colorGrid,
                    edit,
                    Stretch()
                }),
                LayoutV({
                    new QLabel("<b>Color Buttons</b>"),
                    LayoutH({
                        colorButton,
                        colorButton1,
                        colorButton2,
                        popupColorButton,
                        popupColorButton1,
                    }),
                    Stretch(),
                }),
            }).makeWidget());
    }

    ~MainWindow() {}
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("Fusion");

    MainWindow w;
    w.show();

    return a.exec();
}
