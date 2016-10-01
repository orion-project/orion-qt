#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include <QGroupBox>

class ColorSelector : public QGroupBox
{
    Q_OBJECT

public:
    explicit ColorSelector(const QString &title, QWidget *parent = 0);

    const QColor& selectedColor() const;

private:
    class ColorPreview* _preview;

private slots:
    void random();
};

#endif // COLORSELECTOR_H
