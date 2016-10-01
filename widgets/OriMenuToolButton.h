#ifndef ORI_MENU_TOOL_BUTTON_H
#define ORI_MENU_TOOL_BUTTON_H

#include <QToolButton>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

class MenuToolButton : public QToolButton
{
  Q_OBJECT

public:
  MenuToolButton(QWidget *parent = 0);

  void addAction(QAction *action);

private:
  QMenu *menu;

private slots:
  void actionChecked();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_MENU_TOOL_BUTTON_H
