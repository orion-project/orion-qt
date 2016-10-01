#include "OriSettings.h"

#include <QApplication>
#include <QFileInfo>
#include <QWidget>

namespace Ori {

QString Settings::localIniPath()
{
    return qApp->applicationDirPath() % "/" %
           QFileInfo(qApp->applicationFilePath()).baseName().toLower() % ".ini";
}

QSettings* Settings::open()
{
    QString localIni = localIniPath();
    if (QFileInfo(localIni).exists())
        return new QSettings(localIni, QSettings::IniFormat);

    QString app = qApp->applicationName();
    if (app.isEmpty()) return new QSettings; // Qt knows what to do
    QString org = qApp->organizationName();
    if (org.isEmpty()) org = app;
    return new QSettings(QSettings::IniFormat, QSettings::UserScope, org, app);
}

void Settings::endGroups(QSettings *settings)
{
    while (!settings->group().isEmpty()) settings->endGroup();
}

void Settings::beginGroup(QSettings *settings, const QString& name)
{
    endGroups(settings);
    settings->beginGroup(name);
}


Settings::Settings()
{
    _settings = open();
    beginDefaultGroup();
}

Settings::~Settings()
{
    delete _settings;
}

void Settings::storeWindowGeometry(QWidget* w)
{
    storeWindowGeometry(w->objectName(), w);
}

void Settings::storeWindowGeometry(const QString& key, QWidget* w)
{
    if (key.isEmpty()) return;
    beginGroup(_settings, "WindowStates");
    _settings->beginGroup(key);
    _settings->setValue("maximized", w->isMaximized());
    if (!w->isMaximized())
        _settings->setValue("geometry", w->geometry());
}

void Settings::restoreWindowGeometry(QWidget* w, QSize defSize)
{
    restoreWindowGeometry(w->objectName(), w, defSize);
}

void Settings::restoreWindowGeometry(const QString &key, QWidget* w, QSize defSize)
{
    if (key.isEmpty()) return;
    beginGroup(_settings, "WindowStates");
    _settings->beginGroup(key);
    QRect g = _settings->value("geometry").toRect();
    if (g.width() > 0 && g.height() > 0)
        w->setGeometry(g);
    else if (defSize.width() > 0 && defSize.height() > 0)
        w->resize(defSize); // stay default position
    if (_settings->value("maximized").toBool())
        w->setWindowState(w->windowState() | Qt::WindowMaximized);
}

QString Settings::strValue(const QString& key, const QString& value)
{
    return _settings->value(key, value).toString();
}

void Settings::setValue(const QString& key, const QVariant& value)
{
    _settings->setValue(key, value);
}

QVariant Settings::value(const QString& key, const QVariant& def)
{
    return _settings->value(key, def);
}

void Settings::storeWindow(const QString& key, QWidget* w)
{
    Settings().storeWindowGeometry(key, w);
}

void Settings::storeWindow(QWidget* w)
{
    Settings().storeWindowGeometry(w);
}

void Settings::restoreWindow(const QString& key, QWidget* w, QSize defSize)
{
    Settings().restoreWindowGeometry(key, w, defSize);
}

void Settings::restoreWindow(QWidget* w, QSize defSize)
{
    Settings().restoreWindowGeometry(w, defSize);
}

} // namespace Ori
