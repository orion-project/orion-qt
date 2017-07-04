#ifndef ORI_SETTINGS_H
#define ORI_SETTINGS_H

#include <QSettings>
#include <QSize>
#include <memory>

class Settings;

#ifdef __GXX_EXPERIMENTAL_CXX0X__ // TODO condition
typedef std::unique_ptr<QSettings> QSettingsPtr;
#else
typedef std::auto_ptr<QSettings> QSettingsPtr;
#endif

QT_BEGIN_NAMESPACE
class QMainWindow;
QT_END_NAMESPACE

namespace Ori {

/** Thin wrapper around QSettings.
    By default configures QSettings to use ini-files instead of Windows registry.
    Can look for local ini-file nearby an applicaion allowing program be 'portable'.
*/
class Settings
{
public:
    static QString localIniPath();

    /// If local settings file exists - use it meaning 'portable' version
    /// of application. Else open or create settings file in user profile.
    static QSettings* open();

    /// Closes all groups till the root
    static void endGroups(QSettings* settings);

    /// Closes all groups till the root and opens a new group
    static void beginGroup(QSettings *settings, const QString& name);

public:
    Settings();
    ~Settings();

    QSettings* settings() const { return _settings; }

    void storeWindowGeometry(QWidget*);
    void restoreWindowGeometry(QWidget*, QSize defSize = QSize());
    void storeWindowGeometry(const QString& key, QWidget*);
    void restoreWindowGeometry(const QString& key, QWidget*, QSize defSize = QSize());

    void storeDockState(QMainWindow*);
    void restoreDockState(QMainWindow*);
    void storeDockState(const QString& key, QMainWindow*);
    void restoreDockState(const QString& key, QMainWindow*);

    void endGroups() { endGroups(_settings); }
    void beginGroup(const QString& name) { beginGroup(_settings, name); }
    void beginDefaultGroup() { beginGroup(_settings, "Common"); }
    void resetGroup() { endGroups(); beginDefaultGroup(); }

    QString strValue(const QString& key, const QString& value = QString());

    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key, const QVariant& def = QVariant());

public:
    static void storeWindow(const QString& key, QWidget* w) {
        Settings().storeWindowGeometry(key, w);
    }
    static void restoreWindow(const QString& key, QWidget* w, QSize defSize = QSize()) {
        Settings().restoreWindowGeometry(key, w, defSize);
    }
    static void storeWindow(QWidget* w) {
        Settings().storeWindowGeometry(w);
    }
    static void restoreWindow(QWidget* w, QSize defSize = QSize()) {
        Settings().restoreWindowGeometry(w, defSize);
    }

    static void storeDocks(const QString& key, QMainWindow* w) { Settings().storeDockState(key, w); }
    static void restoreDocks(const QString& key, QMainWindow* w) { Settings().restoreDockState(key, w); }
    static void storeDocks(QMainWindow* w) { Settings().storeDockState(w); }
    static void restoreDocks(QMainWindow* w) { Settings().storeDockState(w); }

private:
    QSettings* _settings;
};

} // namespace Ori

#endif // ORI_SETTINGS_H
