#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSpinBox>
#include <QRadioButton>
#include <QTableWidget>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private:
    QRadioButton *simpleRnd, *paramsRnd;
    QSpinBox *darkenThan, *lightenThan, *minDistance;
    QSpinBox *colCount, *rowCount;
    QTableWidget *palette;

    void adjustCellSizes();
    void generate();
};

#endif // MAINWINDOW_H
