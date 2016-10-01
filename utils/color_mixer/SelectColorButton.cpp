#include "SelectColorButton.h"

#include <QColorDialog>

SelectColorButton::SelectColorButton(QWidget *parent) : QPushButton(parent)
{
    setText("Select color...");

    connect(this, SIGNAL(clicked(bool)), this, SLOT(clicked()));
}

void SelectColorButton::clicked()
{
    _selectedColor = QColorDialog::getColor(Qt::white, this);
    emit colorSelected(_selectedColor);
}
