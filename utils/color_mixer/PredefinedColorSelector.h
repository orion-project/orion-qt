#ifndef PREDEFINEDCOLORSELECTOR_H
#define PREDEFINEDCOLORSELECTOR_H

#include <QComboBox>

class PredefinedColorSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit PredefinedColorSelector(QWidget *parent = 0);

    const QColor& selectedColor() const { return _selectedColor; }

signals:
    void colorSelected(const QColor& color);

private slots:
    void itemSelected(int index);

private:
    QColor _selectedColor;
};

#endif // PREDEFINEDCOLORSELECTOR_H
