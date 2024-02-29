#ifndef ORI_MDI_TOOL_BAR_H
#define ORI_MDI_TOOL_BAR_H

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QMdiArea;
class QMdiSubWindow;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

class MdiToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit MdiToolBar(const QString& title, QMdiArea *parent);
    explicit MdiToolBar(QMdiArea *parent);
    bool flat;
    QMenu *menuForButton = nullptr;
    QMenu *menuForSpace = nullptr;
    QMdiSubWindow *windowUnderMenu = nullptr;

public slots:
    void subWindowActivated(QMdiSubWindow*);

protected:
    void paintEvent(QPaintEvent*);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QMdiArea *mdiArea;
    QList<QAction*> actions;

    void uncheckAll();
    QAction* findActionForWindow(QMdiSubWindow *window);

private slots:
    void subWindowDestroyed(QObject*);
    void subWindowTitleChanged(const QString& title);
    void subWindowIconChanged(const QIcon& icon);
    void setActiveSubWindow();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_MDI_TOOL_BAR_H
