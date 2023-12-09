#ifndef ORI_WINDOWS_H
#define ORI_WINDOWS_H

#include <QString>
#include <QSize>

QT_BEGIN_NAMESPACE
class QRect;
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
void setGeometry(QWidget* w, const QRect& g, bool maximized, const QSize& defSize = {});

} // namespace Window
} // namespace Ori

#endif // ORI_WINDOWS_H
