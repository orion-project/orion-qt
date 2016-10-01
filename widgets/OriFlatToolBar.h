#ifndef ORI_FLAT_TOOLBAR_H
#define ORI_FLAT_TOOLBAR_H

#include <QToolBar>

namespace Ori {
namespace Widgets {

class FlatToolBar : public QToolBar
{
public:
    explicit FlatToolBar(QWidget* parent = 0): QToolBar(parent) {}
    explicit FlatToolBar(const QString& title, QWidget* parent = 0): QToolBar(title, parent) {}

protected:
    void paintEvent(QPaintEvent*) { /* do nothing */ }
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_FLAT_TOOLBAR_H
