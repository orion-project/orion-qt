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

protected:
    void paintEvent(QPaintEvent*);

private:
    QMdiArea *mdiArea;
    QList<QAction*> actions;

    void uncheckAll();

private slots:
    void subWindowActivated(QMdiSubWindow*);
    void subWindowDestroyed(QObject*window);
    void subWindowClosing();
    void setActiveSubWindow();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_MDI_TOOL_BAR_H
