#ifndef WIDGET_H
#define WIDGET_H

#include "ColorSelector.h"
#include "ColorPreview.h"
#include "widgets/OriValueEdit.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private slots:
    void mix();

private:
    ColorPreview* _preview;
    ColorSelector* _colorSelector1;
    ColorSelector* _colorSelector2;
    Ori::Widgets::ValueEdit* _factor;
};

#endif // WIDGET_H
