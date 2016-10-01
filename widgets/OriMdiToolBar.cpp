#include "OriMdiToolBar.h"

#include <QAction>
#include <QMdiArea>
#include <QMdiSubWindow>

namespace Ori {
namespace Widgets {

MdiToolBar::MdiToolBar(QMdiArea *parent) : MdiToolBar(QString(), parent)
{
}

MdiToolBar::MdiToolBar(const QString& title, QMdiArea *parent) : QToolBar(parent), flat(false), mdiArea(parent)
{
    setWindowTitle(title);
    setObjectName("toolBar_" % title);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIconSize(QSize(16, 16));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(subWindowActivated(QMdiSubWindow*)));
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

    QVariant wnd = QVariant::fromValue((void*)window);
    for (int i = 0; i < actions.size(); i++)
        if (actions[i]->data() == wnd)
        {
            actions[i]->setChecked(true);
            return;
        }

    connect(window, SIGNAL(closing()), this, SLOT(subWindowClosing()));
    connect(window, SIGNAL(destroyed(QObject*)), this, SLOT(subWindowDestroyed(QObject*)));

    QAction *action = new QAction(this);
    action->setText(window->windowTitle());
    action->setIcon(window->windowIcon());
    action->setCheckable(true);
    action->setChecked(true);
    action->setData(wnd);
    connect(action, SIGNAL(triggered()), this, SLOT(setActiveSubWindow()));
    actions.append(action);
    addAction(action);
}

void MdiToolBar::subWindowClosing()
{
    uncheckAll();
}

void MdiToolBar::subWindowDestroyed(QObject *window)
{
    QVariant wnd = QVariant::fromValue((void*)window);
    for (int i = 0; i < actions.size(); i++)
        if (actions[i]->data() == wnd)
        {
            delete actions[i];
            actions.removeAt(i);
            break;
        }
}

void MdiToolBar::setActiveSubWindow()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action->isChecked())
    {
        QMdiSubWindow *window = (QMdiSubWindow*)(action->data().value<void*>());
        if (!window->isVisible()) window->show();
        if (window->windowState() | Qt::WindowMinimized) window->showNormal();
        mdiArea->setActiveSubWindow(window);
    }
    else
        mdiArea->setActiveSubWindow(0);
}

} // namespace Widgets
} // namespace Ori
