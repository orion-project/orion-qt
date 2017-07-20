#include "MainWindow.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriTools.h"
#include "helpers/OriWindows.h"
#include "helpers/OriWidgets.h"

#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>

using namespace Ori::Layouts;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    simpleRnd = new QRadioButton("Simple random");
    simpleRnd->setChecked(true);

    paramsRnd = new QRadioButton("Parameterized random");
    darkenThan = Ori::Gui::spinBox(0, 255, 200);
    lightenThan = Ori::Gui::spinBox(0, 255, 20);
    minDistance = Ori::Gui::spinBox(0, 255, 20);
    auto rndParamsLayout = new QFormLayout;
    rndParamsLayout->addRow(new QLabel("Darken than"), darkenThan);
    rndParamsLayout->addRow(new QLabel("Lighten Than"), lightenThan);
    rndParamsLayout->addRow(new QLabel("Min distance"), minDistance);

    colCount = Ori::Gui::spinBox(1, 100, 10);
    rowCount = Ori::Gui::spinBox(1, 100, 10);
    auto countLayout = new QFormLayout;
    countLayout->addRow(new QLabel("Cols"), colCount);
    countLayout->addRow(new QLabel("Rwos"), rowCount);

    auto buttonGenerate = new QPushButton("Generate");
    connect(buttonGenerate, &QPushButton::clicked, this, &MainWindow::generate);

    palette = new QTableWidget;
    palette->verticalHeader()->hide();
    palette->horizontalHeader()->hide();
    palette->setRowCount(rowCount->value());
    palette->setColumnCount(colCount->value());
    adjustCellSizes();

    LayoutH({
                LayoutV({
                    simpleRnd,
                    Space(16),

                    paramsRnd,
                    rndParamsLayout,
                    Space(16),

                    new QLabel("Colors count"),
                    countLayout,
                    Space(16),

                    buttonGenerate,
                    Stretch()
                }),
                palette
            })
            .useFor(this);

    Ori::Wnd::moveToScreenCenter(this);
}

void MainWindow::adjustCellSizes()
{
    for (int r = 0; r < palette->rowCount(); r++)
        palette->setRowHeight(r, 24);
    for (int c = 0; c < palette->columnCount(); c++)
        palette->setColumnWidth(c, 24);
}

void MainWindow::generate()
{
    int rows = rowCount->value();
    int cols = colCount->value();

    palette->setRowCount(rows);
    palette->setColumnCount(cols);
    adjustCellSizes();

    bool simple = simpleRnd->isChecked();
    int darken = darkenThan->value();
    int lighten = lightenThan->value();
    int distance = minDistance->value();

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
        {
            auto item = palette->item(r, c);
            if (!item)
            {
                item = new QTableWidgetItem;
                palette->setItem(r, c, item);
            }
            auto color = simple
                    ? Ori::Color::random()
                    : Ori::Color::random(darken, lighten, distance);
            item->setBackgroundColor(color);
            item->setToolTip(QString("%1\n%2\n%3")
                             .arg(Ori::Color::formatHtml(color))
                             .arg(Ori::Color::formatRgb(color))
                             .arg(Ori::Color::formatHsl(color))
                             );
        }
}
