#include "OriWindows.h"

#include <QApplication>
#include <QFileInfo>
#include <QIcon>
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

} // namespace Window
} // namespace Ori

