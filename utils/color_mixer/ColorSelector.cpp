#include "ColorSelector.h"
#include "ColorPreview.h"
#include "PaletteColorSelector.h"
#include "PredefinedColorSelector.h"
#include "SelectColorButton.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"

#include <QLabel>

ColorSelector::ColorSelector(const QString& title, QWidget *parent) : QGroupBox(title, parent)
{
    auto paletteColorSelector = new PaletteColorSelector;
    _preview = new ColorPreview;
    auto selectColorButton = new SelectColorButton;
    auto predefinedColorSelector = new PredefinedColorSelector;
    connect(selectColorButton, SIGNAL(colorSelected(QColor)), _preview, SLOT(setColor(QColor)));
    connect(paletteColorSelector, SIGNAL(colorSelected(QColor)), _preview, SLOT(setColor(QColor)));
    connect(predefinedColorSelector, SIGNAL(colorSelected(QColor)), _preview, SLOT(setColor(QColor)));

    Ori::Gui::layoutV(this, {
        new QLabel("From palette:"),
        paletteColorSelector,
        new QLabel("or"),
        new QLabel("Predefined color:"),
        predefinedColorSelector,
        new QLabel("or"),
        new QLabel("Custom color:"),
        selectColorButton,
        new QLabel("or"),
        Ori::Gui::button("Random", this, SLOT(random())),
        Ori::Gui::defaultSpacing(),
        _preview
    });
}

const QColor& ColorSelector::selectedColor() const
{
    return _preview->color();
}

void ColorSelector::random()
{
    _preview->setColor(Ori::Color::random());
}
