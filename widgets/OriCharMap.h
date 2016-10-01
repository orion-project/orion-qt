#ifndef ORI_CHAR_MAP_H
#define ORI_CHAR_MAP_H

#include <QFont>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

#include <QTextOption>
#include <QScrollBar>

namespace Ori {
namespace Widgets {

class CharMap : public QWidget
{
    Q_OBJECT

public:
    CharMap(QWidget *parent = 0);
    QSize sizeHint() const;

public slots:
    void setFont(const QFont &font);
    void setSize(const QString &fontSize);
    void setStyle(const QString &fontStyle);
    void setTopRow(int value);

signals:
    void characterSelected(const QString &character);

protected:
    void mouseMoveEvent(class QMouseEvent *);
    void mousePressEvent(class QMouseEvent *event);
    void paintEvent(class QPaintEvent *);
    void resizeEvent(class QResizeEvent *);
    void wheelEvent(class QWheelEvent *event);

private:
    QFont displayFont;
    int lastKey;
    float cellW, cellH;
    int gridW, gridH;
    int rowCount, colCount; // columns count and _visible_ rows count
    int topRow, totalRows; // total row count for all chars in the font
    QTextOption option;
    QScrollBar scrollBar;
    QList<int> chars;

    void updateView(bool updateChars = false);
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_CHAR_MAP_H
