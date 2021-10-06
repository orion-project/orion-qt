#ifndef ORI_LAYOUTS_H
#define ORI_LAYOUTS_H

#include <QWidget>
#include <QBoxLayout>

namespace Ori {
namespace Layouts {

enum class LayoutItemMode { Layout, Widget, Stretch, Space };

class LayoutItem
{
public:
    LayoutItem(QWidget* widget) { _mode = LayoutItemMode::Widget; _widget = widget; }
    LayoutItem(QLayout* layout) { _mode = LayoutItemMode::Layout; _layout = layout; }

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


class Space : public LayoutItem
{
public:
    Space(int size) { _mode = LayoutItemMode::Space; _space = size; }
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

    LayoutBox& setMargin(int value) { _layout->setContentsMargins(value, value, value, value); return *this; }
    LayoutBox& setSpacing(int value) { boxLayout()->setSpacing(value); return *this; }
    LayoutBox& setStretchFactor(QWidget* w, int s) { boxLayout()->setStretchFactor(w, s); return *this; }
    LayoutBox& useFor(QWidget* parent) { parent->setLayout(_layout); return *this; }
    QWidget* makeWidget() { auto w = new QWidget; w->setLayout(_layout); return w; }

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
