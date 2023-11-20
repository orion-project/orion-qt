#include "MainWindow.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"
#include "tools/OriStyler.h"
#include "widgets/OriStylesMenu.h"

#include <QDateTime>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    qsrand(QDateTime::currentMSecsSinceEpoch());
#endif

    menuBar()->addMenu(new Ori::Widgets::StylesMenu(new Ori::Styler(this)));

    auto content = new QWidget;

    Ori::Gui::layoutH(content,
    {
        _colorSelector1 = new ColorSelector("Color 1"),
        Ori::Gui::groupV(QString("Mixed color"),
        {
             (QObject*)(Ori::Gui::layoutH(
             {
                 new QLabel("Factor:"),
                 _factor = new Ori::Widgets::ValueEdit,
                 Ori::Gui::button("Mix", this, SLOT(mix())),
             })),
             _preview = new ColorPreview()
         }),
        _colorSelector2 = new ColorSelector("Color 2"),
    });

    _factor->setValue(0.5);

    setCentralWidget(content);
}

void MainWindow::mix()
{
    _preview->setColor(Ori::Color::blend(_colorSelector1->selectedColor(),
                                         _colorSelector2->selectedColor(),
                                         _factor->value()));
}
