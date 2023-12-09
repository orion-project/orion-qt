#include "OriSettings.h"

#include "../helpers/OriWindows.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QMainWindow>
#include <QWidget>

namespace Ori {

class GroupResetAndBackup
{
public:
    GroupResetAndBackup(QSettings* settings) : _settings(settings)
    {
        while (!settings->group().isEmpty())
        {
            _groups << settings->group();
            settings->endGroup();
        }
    }

    ~GroupResetAndBackup()
    {
        while (!_settings->group().isEmpty())
            _settings->endGroup();

        for (int i = _groups.size()-1; i >= 0; i--)
            _settings->beginGroup(_groups.at(i));
    }

private:
    QSettings* _settings;
    QStringList _groups;
};



QString Settings::localIniPath()
{
    return qApp->applicationDirPath() % "/" %
           QFileInfo(qApp->applicationFilePath()).baseName().toLower() % ".ini";
}

QSettings* Settings::open()
{
    QString localIni = localIniPath();
    if (QFileInfo::exists(localIni))
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
    SettingsHelper::storeWindowGeometry(_settings, w, w->objectName());
}

void Settings::restoreWindowGeometry(QWidget* w, const QSize& defSize)
{
    SettingsHelper::restoreWindowGeometry(_settings, w, w->objectName(), defSize);
}

void Settings::storeWindowGeometry(const QString& key, QWidget* w)
{
    SettingsHelper::storeWindowGeometry(_settings, w, key);
}

void Settings::restoreWindowGeometry(const QString &key, QWidget* w, const QSize& defSize)
{
    SettingsHelper::restoreWindowGeometry(_settings, w, key, defSize);
}

namespace SettingsHelper {
void storeWindowGeometry(QSettings* s, QWidget* w, const QString& key)
{
    auto group = key.isEmpty() ? w->objectName() : key;
    if (group.isEmpty()) return;

    GroupResetAndBackup backup(s);

    s->beginGroup("WindowStates");
    s->beginGroup(group);
    s->setValue("maximized", w->isMaximized());
    if (!w->isMaximized())
        s->setValue("geometry", w->geometry());

}

void restoreWindowGeometry(QSettings* s, QWidget* w, const QString& key, const QSize& defSize)
{
    auto group = key.isEmpty() ? w->objectName() : key;
    if (group.isEmpty()) return;

    GroupResetAndBackup backup(s);
    s->beginGroup("WindowStates");
    s->beginGroup(group);
    Ori::Wnd::setGeometry(w, s->value("geometry").toRect(), s->value("maximized").toBool(), defSize);
}
} // namespace SettingsHelper


void Settings::storeDockState(QMainWindow* w) { storeDockState(w->objectName(), w); }
void Settings::restoreDockState(QMainWindow* w) { restoreDockState(w->objectName(), w); }

void Settings::storeDockState(const QString& key, QMainWindow* w)
{
    if (key.isEmpty()) return;

    GroupResetAndBackup backup(_settings);

    _settings->beginGroup("WindowStates");
    _settings->beginGroup(key);
    _settings->setValue("toolbars", w->saveState());
}

void Settings::restoreDockState(const QString& key, QMainWindow* w)
{
    if (key.isEmpty()) return;

    GroupResetAndBackup backup(_settings);

    _settings->beginGroup("WindowStates");
    _settings->beginGroup(key);
    w->restoreState(_settings->value("toolbars").toByteArray());
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

} // namespace Ori
