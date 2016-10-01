#ifndef SELECTCOLORBUTTON_H
#define SELECTCOLORBUTTON_H

#include <QPushButton>

class SelectColorButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SelectColorButton(QWidget *parent = 0);

    const QColor& selectedColor() const { return _selectedColor; }

signals:
    void colorSelected(const QColor& color);

private slots:
    void clicked();

private:
    QColor _selectedColor;
};

#endif // SELECTCOLORBUTTON_H
