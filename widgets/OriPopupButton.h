#ifndef ORI_POPUP_BUTTON_H
#define ORI_POPUP_BUTTON_H

#include <QToolButton>

namespace Ori {
namespace Widgets {

class PopupButton : public QToolButton
{
    Q_OBJECT

public:
    PopupButton(QWidget *parent = 0);
    ~PopupButton() {}

    void setPopup(class PopupGrid*);

protected:
    void mousePressEvent(QMouseEvent*);

protected slots:
    void popupClosed();

private:
    class PopupGrid *mPopup;
};

class PopupGrid : public QWidget
{
    Q_OBJECT

public:
    PopupGrid(QWidget *parent = 0);
    ~PopupGrid() {}

    void addItem(const QString &item);
    void setItems(const QStringList &items, int colCount);
    void setColCount(int value);
    void setCellSize(int value);

    bool justOpened;

public slots:
    void show(const QPoint &at);

signals:
    void closed();
    void selected(const QString&);

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    int mColCount, mRowCount, mCellSize;
    QStringList mItems;

    int hoverIndex, pressIndex;
    int frameW, marginH, marginV;

    int indexAtPos(const QPoint &pos);
    void storeMetrics();
    void calcRowCount();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_POPUP_BUTTON_H
