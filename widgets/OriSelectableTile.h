#ifndef ORI_SELECTABLE_TILE_H
#define ORI_SELECTABLE_TILE_H

#include <QFrame>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

class SelectableTile : public QFrame
{
    Q_OBJECT

public:
    explicit SelectableTile(QWidget *parent = nullptr);

    void setPixmap(const QPixmap& pixmap);

    void setTitle(const QString& title);
    void setTitleStyleSheet(const QString& styleSheet);

    QVariant data() const { return _data; }
    void setData(const QVariant& data);

    void setSelected(bool on);
    bool isSelected() const { return _selected; }

    bool selectionFollowsFocus = false;

signals:
    void selected();
    void goingSelectNext();
    void goingSelectPrev();
    void doubleClicked();

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QLabel *_iconLabel, *_titleLabel;
    QWidget* _content;
    QVariant _data;
    bool _selected = false;

    void select(bool raiseEvent);
    void updateColor();
};


class SelectableTileRadioGroup : public QObject
{
    Q_OBJECT

public:
    explicit SelectableTileRadioGroup(QObject *parent = nullptr);

    void addTile(SelectableTile* tile);

    QVariant selectedData() const;
    void selectData(const QVariant& data);

    bool selectionFollowsArrows = true;

signals:
    void dataSelected(const QVariant& data);
    void doubleClicked(const QVariant& data);

private:
    QList<SelectableTile*> _tiles;

    void tileSelected();
    void selectNextTile();
    void selectPrevTile();
    void tileDoubleClicked();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_SELECTABLE_TILE_H
