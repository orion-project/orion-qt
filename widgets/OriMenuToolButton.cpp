#include "OriMenuToolButton.h"

#include <QAction>
#include <QDebug>
#include <QMenu>

namespace Ori {
namespace Widgets {

MenuToolButton::MenuToolButton(QWidget *parent) : QToolButton(parent)
{
    _menu = new QMenu(this);

    setMenu(_menu);
    setPopupMode(QToolButton::InstantPopup);
}

void MenuToolButton::addAction(QAction *action)
{
    _menu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(actionChecked()));
}

void MenuToolButton::addAction(int id, QAction *action)
{
    if (!_group)
        _group = new QActionGroup(this);
    _group->addAction(action);
    action->setData(id);
    action->setCheckable(true);
    addAction(action);
}

void MenuToolButton::addAction(int id, const QString& title, const QString& icon, const QString &tooltip)
{
    if (!_group)
        _group = new QActionGroup(this);
    auto action = new QAction(title, _group);
    if (!icon.isEmpty())
        action->setIcon(QIcon(icon));
    if (!tooltip.isEmpty())
        action->setToolTip(tooltip);
    action->setData(id);
    action->setCheckable(true);
    addAction(action);
}

void MenuToolButton::actionChecked()
{
    setDefaultAction(qobject_cast<QAction*>(sender()));
}

int MenuToolButton::selectedId() const
{
    if (!_group) return 0;

    foreach (auto a, _group->actions())
        if (a->isChecked())
            return a->data().toInt();
    return 0;
}

void MenuToolButton::setSelectedId(int id)
{
    if (!_group) return;

    foreach (auto a, _group->actions())
        if (a->data().toInt() == id) {
            a->setChecked(true);
            setDefaultAction(a);
            return;
        }
}

int MenuToolButton::selectedFlags(int origFlags) const
{
    if (!_group) return origFlags;

    int flags = origFlags;
    foreach (auto a, _group->actions()) {
        int f = a->data().toInt();
        if (a->isChecked())
            flags |= f;
        else
            flags &= ~f;
    }
    return flags;
}

void MenuToolButton::setSelectedFlags(int flags)
{
    if (!_group) return;

    foreach (auto a, _group->actions()) {
        if (flags & a->data().toInt()) {
            a->setChecked(true);
            setDefaultAction(a);
            return;
        }
    }
}

} // namespace Widgets
} // namespace Ori
