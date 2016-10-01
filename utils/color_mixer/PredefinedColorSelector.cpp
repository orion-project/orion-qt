#include "PredefinedColorSelector.h"

#define ADD_ITEM(item) addItem(# item, int(Qt::item));

PredefinedColorSelector::PredefinedColorSelector(QWidget *parent) : QComboBox(parent)
{
    ADD_ITEM(color0)
    ADD_ITEM(color1)
    ADD_ITEM(black)
    ADD_ITEM(white)
    ADD_ITEM(darkGray)
    ADD_ITEM(gray)
    ADD_ITEM(lightGray)
    ADD_ITEM(red)
    ADD_ITEM(green)
    ADD_ITEM(blue)
    ADD_ITEM(cyan)
    ADD_ITEM(magenta)
    ADD_ITEM(yellow)
    ADD_ITEM(darkRed)
    ADD_ITEM(darkGreen)
    ADD_ITEM(darkBlue)
    ADD_ITEM(darkCyan)
    ADD_ITEM(darkMagenta)
    ADD_ITEM(darkYellow)
    ADD_ITEM(transparent)

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(itemSelected(int)));
}

void PredefinedColorSelector::itemSelected(int index)
{
    _selectedColor = QColor((Qt::GlobalColor)itemData(index).toInt());
    emit colorSelected(_selectedColor);
}

