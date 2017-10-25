#ifndef ORI_WINDOWS_H
#define ORI_WINDOWS_H

#include <QString>

QT_BEGIN_NAMESPACE
class QScreen;
class QWidget;
QT_END_NAMESPACE

namespace Ori {
namespace Wnd {

QWidget *initWindow(QWidget*, const QString& title, const QString& iconPath, bool autoDelete = true);

void setWindowIcon(QWidget*, const QString&);
void setWindowProject(QWidget*, const QString&);
void setWindowFilePath(QWidget*, const QString&);

void moveToScreenCenter(QWidget*, QWidget* = nullptr);
QScreen* findScreenOrPrimary(QWidget* w);

} // namespace Window
} // namespace Ori

#endif // ORI_WINDOWS_H
