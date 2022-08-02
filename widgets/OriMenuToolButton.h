#ifndef ORI_MENU_TOOL_BUTTON_H
#define ORI_MENU_TOOL_BUTTON_H

#include <QToolButton>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
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
  void addAction(int id, const QString& title, const QString& icon, const QString& tooltip = QString());

  int selectedId() const;
  void setSelectedId(int id);

  // This supposes that ids can be ORed (this is true for Qt::Align* flags)
  int selectedFlags(int origFlags) const;
  void setSelectedFlags(int flags);

private:
  QMenu *_menu;
  QActionGroup *_group = nullptr;

private slots:
  void actionChecked();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_MENU_TOOL_BUTTON_H
