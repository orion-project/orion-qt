#include "OriMenuToolButton.h"

#include <QAction>
#include <QMenu>

namespace Ori {
namespace Widgets {

MenuToolButton::MenuToolButton(QWidget *parent) : QToolButton(parent)
{
    menu = new QMenu(this);
    setMenu(menu);
    setPopupMode(QToolButton::InstantPopup);
}

void MenuToolButton::addAction(QAction *action)
{
    menu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(actionChecked()));
}

void MenuToolButton::actionChecked()
{
    setDefaultAction(qobject_cast<QAction*>(sender()));
}

} // namespace Widgets
} // namespace Ori
