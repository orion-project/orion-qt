#include "OriWindows.h"

#include <QApplication>
#include <QDebug>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QDesktopWidget>
#endif
#include <QFileInfo>
#include <QIcon>
#include <QScreen>
#include <QWidget>
#include <QWindow>

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
        setWindowProject(window, QFileInfo(fileName).completeBaseName());
}

QScreen* findScreenOrPrimary(QWidget* w)
{
    if (!w)
        return QGuiApplication::primaryScreen();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    return QGuiApplication::screenAt(w->geometry().topLeft());
#else
    int screenNumber = qApp->desktop()->screenNumber(w);
    if (screenNumber < 0)
        return QGuiApplication::primaryScreen();
    return QGuiApplication::screens().at(screenNumber);
#endif
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

void setGeometry(QWidget* w, const QRect& g, bool maximized, const QSize& defSize)
{
    if (g.width() > 0 and g.height() > 0)
    {
        bool screenFound = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    #if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)) and (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
        // It's something broken with multi-screen handling in earlier versions of Qt6
        // (not sure about exact versions 6.2 and 6.7, it's just what's been checked)
        // It can't find screenAt when the saved geometry is outside of the main screen
        // There is also something wrong with saved geometry when window is moved between screens
        // But this workaround works at least when geometry is saved-restored on the same non-main screen
        foreach (auto s, qApp->screens()) {
            if (s->virtualGeometry().contains(g.topLeft())) {
                screenFound = true;
                w->setScreen(s);
                w->setGeometry(g);
                break;
            }
        }
    #else
        if (QGuiApplication::screenAt(g.topLeft())) {
            screenFound = true;
            w->setGeometry(g);
        }
    #endif
        if (!screenFound) {
            qWarning() << "Stored geometry is out of available screens, ignoring" << w->objectName() << g;
            if (defSize.width() > 0 and defSize.height() > 0)
                w->resize(defSize);
            // This function is mostly called from constructors and window size is not yet fully defined there,
            // e.g. at this moment window size could be 640x480, which is probably some QWidget's default value,
            // and eventually the window gets offset from the creen center when it's size calculated properly.
            // So it's better to provide default size, to have the window initially centered.
            Ori::Wnd::moveToScreenCenter(w, qApp->activeWindow());
        }
#else
        w->setGeometry(g);
#endif
    }
    else
    {
        if (defSize.width() > 0 and defSize.height() > 0)
            w->resize(defSize);
        moveToScreenCenter(w, qApp->activeWindow());
    }
    if (maximized)
        w->setWindowState(w->windowState() | Qt::WindowMaximized);
}

} // namespace Window
} // namespace Ori

