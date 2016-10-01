#ifndef ORI_MRU_LIST_H
#define ORI_MRU_LIST_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QSettings;
QT_END_NAMESPACE

namespace Ori {

////////////////////////////////////////////////////////////////////////////////

class MruList : public QObject
{
    Q_OBJECT

public:
    explicit MruList(QObject *parent = 0);

    /// Load mru-list from default settings group and specified or default key
    /// Default group and key later used for saving list when new item is appended.
    void load(const QString &key = QString());

    /// Load mru-list from currently opened settings group and specified or default key.
    /// Opened group and key are preserved and later used for saving list when new item is appended.
    void load(QSettings* settings, const QString &key = QString());

    QStringList items() const;
    const QList<QAction*>& actions() const { return _actions; }
    QAction* actionClearAll() const { return _actionClearAll; }
    QAction* actionClearInvalids() const { return _actionClearInvalids; }

public slots:
    void append(const QString& item);

signals:
    void clicked(const QString& item);
    void changed();

protected:
    virtual bool sameItems(const QString& item1, const QString& item2) const;
    virtual bool validateItem(const QString&) const { return true; }
    virtual bool canClick(const QString&) const { return true; }

private:
    QString _settingGroup, _settingsKey;
    QList<QAction*> _actions;
    QAction *_actionClearAll, *_actionClearInvalids;

    void update();
    void save();

    int indexOf(const QString& item) const;
    QAction* action(const QString& item) const;
    QAction* makeAction(const QString& item);
    QList<QAction*> invalidItems() const;

private slots:
    void actionTriggered();
    void clearInvalids();
    void clearAll();
};

////////////////////////////////////////////////////////////////////////////////

class MruFileList : public MruList
{
    Q_OBJECT

public:
    explicit MruFileList(QObject *parent = 0) : MruList(parent) {}

protected:
    bool sameItems(const QString& item1, const QString& item2) const override;
    bool validateItem(const QString& item) const override;
    bool canClick(const QString& item) const override;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace Ori

#endif // ORI_MRU_LIST_H

