#ifndef ORI_CONFIG_DLG_H
#define ORI_CONFIG_DLG_H

#include "OriBasicConfigDlg.h"

#include <QMap>

namespace Ori {
namespace Dlg {

struct ConfigPage
{
    ConfigPage(int id, const QString& title, const QString& icon = QString()) : id(id), title(title), iconPath(icon) {}

    ConfigPage& withHelpTopic(const QString& topic) { helpTopic = topic; return *this; }

    int id;
    QString title;
    QString iconPath;

    /// Page help topic.
    /// If page has a topic, then the Help button is displaued in the dialog
    /// and the onHelpRequested handler is called
    QString helpTopic;
};


class ConfigItem
{
public:
    ConfigItem(int pageId, const QString& title) : pageId(pageId), title(title) {}
    virtual ~ConfigItem() {}

    int pageId;
    QString title;
};


class ConfigItemBool : public ConfigItem
{
public:
    ConfigItemBool(int pageId, const QString& title, bool* value) : ConfigItem(pageId,  title), value(value) {}

    bool* value;
};


struct ConfigDlgOpts
{
    /// Dialog window title.
    QString title;

    /// Dialog window icon.
    QString iconPath;

    /// Object name is used to store dialog geometry between executions.
    QString objectName;

    /// Icon size of categories (pages) list in the left panel.
    int pageIconSize = 48;

    /// An id of page to be activated after dialog shown.
    int currentPageId;

    /// Pages (categories) descriptors.
    QVector<ConfigPage> pages;

    /// Configuraton items.
    /// The dialog takes ownership on items, don't use them after dialog closed.
    QVector<ConfigItem*> items;

    /// Help request handler that should be called if page has a help topic
    std::function<void(const QString&)> onHelpRequested;
};

/**
    Generic configuration dialog.

    It takes a list of ConfigItem and creates visual editors for them.
    The dialog takes ownership on ConfigItem-s so don't use them after dialog closed.

    See `exampels/config ` for usage example.
*/
class ConfigDlg : public BasicConfigDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(const ConfigDlgOpts& opts, QWidget* parent = nullptr);
    ~ConfigDlg();

    static bool edit(const ConfigDlgOpts& opts);

    // inherited from BasicConfigDialog
    virtual void populate() override;
    virtual bool collect() override;

private:
    QMap<ConfigItem*, class ConfigItemEditor*> _editors;

    QWidget* makePage(const ConfigPage& page, const ConfigDlgOpts& opts);
};

} // namespace Dlg
} // namespace Ori

#endif // ORI_CONFIG_DLG_H
