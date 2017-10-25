#include "OriWindows.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QIcon>
#include <QScreen>
#include <QWidget>

namespace Ori {
namespace Wnd {

QWidget* initWindow(QWidget* w, const QString& title, const QString& iconPath, bool autoDelete)
{
    w->setWindowTitle(title);
    w->setWindowIcon(QIcon(iconPath));
    if (autoDelete)
        w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void setWindowIcon(QWidget *window, const QString& fileName)
{
#ifdef Q_OS_WIN
    // On Windows an application icon will be used as window icon automatically
    Q_UNUSED(window)
    Q_UNUSED(fileName)
#else
    window->setWindowIcon(QIcon(fileName));
#endif
}

void setWindowProject(QWidget *window, const QString& projectName)
{
    window->setWindowTitle(projectName % QString::fromLatin1("[*] ") % QChar(0x2014) % ' ' % qApp->applicationName());
}

void setWindowFilePath(QWidget *window, const QString& fileName)
{
    if (fileName.isEmpty())
        setWindowProject(window, QApplication::translate("Window title", "Untitled"));
    else
        setWindowProject(window, QFileInfo(fileName).baseName());
}

QScreen* findScreenOrPrimary(QWidget* w)
{
    if (!w)
        return QGuiApplication::primaryScreen();
    int screenNumber = qApp->desktop()->screenNumber(w);
    if (screenNumber < 0)
        return QGuiApplication::primaryScreen();
    return QGuiApplication::screens().at(screenNumber);
}

void moveToScreenCenter(QWidget* w, QWidget* screenOfThisWidget)
{
    auto windowSize = w->size();
    auto screen = findScreenOrPrimary(screenOfThisWidget);
    auto screenRect = screen->availableGeometry();
    auto x = screenRect.width()/2 - windowSize.width()/2;
    auto y = screenRect.height()/2 - windowSize.height()/2;
    w->move(screenRect.left() + x, screenRect.top() + y);
}

} // namespace Window
} // namespace Ori

