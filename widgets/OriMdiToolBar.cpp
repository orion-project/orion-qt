#include "OriMdiToolBar.h"

#include <QAction>
#include <QMdiArea>
#include <QMdiSubWindow>

#define ACTION_DATA(window) QVariant::fromValue((void*)(window))

namespace Ori {
namespace Widgets {

MdiToolBar::MdiToolBar(QMdiArea *parent) : MdiToolBar(QString(), parent)
{
}

MdiToolBar::MdiToolBar(const QString& title, QMdiArea *parent) : QToolBar(parent), flat(false), mdiArea(parent)
{
    setWindowTitle(title);
    setObjectName("toolBar_Windows");
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //setIconSize(QSize(16, 16));

    connect(mdiArea, &QMdiArea::subWindowActivated, this, &MdiToolBar::subWindowActivated);
}

void MdiToolBar::paintEvent(QPaintEvent *event)
{
    if (!flat) QToolBar::paintEvent(event);
}

void MdiToolBar::uncheckAll()
{
    for (int i = 0; i < actions.size(); i++)
        actions[i]->setChecked(false);
}

void MdiToolBar::subWindowActivated(QMdiSubWindow *window)
{
    uncheckAll();

    if (!window) return;

    auto action = findActionForWindow(window);
    if (action)
    {
        action->setChecked(true);
        return;
    }

    connect(window, &QMdiSubWindow::destroyed, this, &MdiToolBar::subWindowDestroyed);
    connect(window, &QMdiSubWindow::windowTitleChanged, this, &MdiToolBar::subWindowTitleChanged);
    connect(window, &QMdiSubWindow::windowIconChanged, this, &MdiToolBar::subWindowIconChanged);

    action = new QAction(this);
    action->setText(window->windowTitle());
    action->setIcon(window->windowIcon());
    action->setCheckable(true);
    action->setChecked(true);
    action->setData(ACTION_DATA(window));
    connect(action, SIGNAL(triggered()), this, SLOT(setActiveSubWindow()));
    actions.append(action);
    addAction(action);
}

void MdiToolBar::subWindowDestroyed(QObject *window)
{
    QVariant wnd = ACTION_DATA(window);
    for (int i = 0; i < actions.size(); i++)
        if (actions[i]->data() == wnd)
        {
            delete actions[i];
            actions.removeAt(i);
            break;
        }
}

QAction* MdiToolBar::findActionForWindow(QMdiSubWindow *window)
{
    QVariant data = ACTION_DATA(window);
    for (auto action : actions)
        if (action->data() == data)
            return action;
    return nullptr;
}

void MdiToolBar::setActiveSubWindow()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action->isChecked())
    {
        QMdiSubWindow *window = qvariant_cast<QMdiSubWindow*>(action->data());
        if (!window->isVisible()) window->show();
        if (window->windowState() | Qt::WindowMinimized) window->showNormal();
        mdiArea->setActiveSubWindow(window);
    }
    else
        mdiArea->setActiveSubWindow(0);
}

void MdiToolBar::subWindowTitleChanged(const QString& title)
{
    auto window = qobject_cast<QMdiSubWindow*>(sender());
    if (!window) return;
    auto action = findActionForWindow(window);
    if (!action) return;
    action->setText(title);
}

void MdiToolBar::subWindowIconChanged(const QIcon& icon)
{
    auto window = qobject_cast<QMdiSubWindow*>(sender());
    if (!window) return;
    auto action = findActionForWindow(window);
    if (!action) return;
    action->setIcon(icon);
}

} // namespace Widgets
} // namespace Ori
