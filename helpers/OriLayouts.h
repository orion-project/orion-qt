#ifndef ORI_LAYOUTS_H
#define ORI_LAYOUTS_H

#include <QWidget>
#include <QBoxLayout>

QT_BEGIN_NAMESPACE
class QGroupBox;
QT_END_NAMESPACE

namespace Ori {
namespace Layouts {

enum class LayoutItemMode { Layout, Widget, Stretch, Space };

class LayoutItem
{
public:
    LayoutItem(QWidget* widget) { _mode = LayoutItemMode::Widget; _widget = widget; }
    LayoutItem(QLayout* layout) { _mode = LayoutItemMode::Layout; _layout = layout; }
    LayoutItem(const QString& label);

    void addTo(QBoxLayout* layout) const
    {
        switch (_mode)
        {
        case LayoutItemMode::Layout:
            if (_layout)
                layout->addLayout(_layout);
            break;

        case LayoutItemMode::Widget:
            if (_widget)
                layout->addWidget(_widget);
            break;

        case LayoutItemMode::Stretch:
            layout->addStretch();
            break;

        case LayoutItemMode::Space:
            layout->addSpacing(_space);
            break;
        }
    }

protected:
    LayoutItem() {}

    LayoutItemMode _mode;
    union
    {
        QLayout* _layout = nullptr;
        QWidget* _widget;
        int _space;
    };
};


class Stretch : public LayoutItem
{
public:
    Stretch() { _mode = LayoutItemMode::Stretch; }
};


/// Gets space as hardcoded value
class Space : public LayoutItem
{
public:
    Space(int size) { _mode = LayoutItemMode::Space; _space = size; }
};


/// Gets space as factor of default layout horizontal spacing
class SpaceH : public LayoutItem
{
public:
    SpaceH(qreal factor = 1);
};


/// Gets space as factor of default layout vertical spacing
class SpaceV : public LayoutItem
{
public:
    SpaceV(qreal factor = 1);
};


typedef const std::initializer_list<LayoutItem> LayoutItems;


class LayoutBox : public LayoutItem
{
public:
    LayoutBox(QBoxLayout* layout, LayoutItems items)
    {
        _mode = LayoutItemMode::Layout;
        _layout = layout;
        add(items);
    }

    LayoutBox& setDefMargins();
    LayoutBox& setMargin(int value) { _layout->setContentsMargins(value, value, value, value); return *this; }
    LayoutBox& setMargins(int left, int top, int right, int bottom) { _layout->setContentsMargins(left, top, right, bottom); return *this; }
    LayoutBox& setSpacing(int value) { boxLayout()->setSpacing(value); return *this; }
    LayoutBox& setDefSpacing(qreal factor = 1);
    LayoutBox& setStretchFactor(QWidget* w, int s) { boxLayout()->setStretchFactor(w, s); return *this; }
    LayoutBox& useFor(QWidget* parent) { parent->setLayout(_layout); return *this; }
    QWidget* makeWidget() { auto w = new QWidget; w->setLayout(_layout); return w; }
    QSharedPointer<QWidget> makeWidgetAuto() { auto w = new QWidget; w->setLayout(_layout); return QSharedPointer<QWidget>(w); }
    QGroupBox* makeGroupBox(const QString& title);

    QBoxLayout* boxLayout() const { return qobject_cast<QBoxLayout*>(_layout); }

    void add(LayoutItem item)
    {
        item.addTo(boxLayout());
    }

    void add(LayoutItems items)
    {
        for (const LayoutItem& item : items)
            item.addTo(boxLayout());
    }
};


class LayoutH : public LayoutBox
{
public:
    LayoutH(LayoutItems items) : LayoutBox(new QHBoxLayout, items) {}
};


class LayoutV : public LayoutBox
{
public:
    LayoutV(LayoutItems items) : LayoutBox(new QVBoxLayout, items) {}
};

} // namespace Ori
} // namespace Layouts

#endif // ORI_LAYOUTS_H
