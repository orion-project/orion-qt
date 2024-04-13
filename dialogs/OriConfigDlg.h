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
    ConfigPage& withMargins(int m) { margin = m; return *this; }
    ConfigPage& withSpacing(int s) { spacing = s; return *this; }

    int id;
    QString title;
    QString iconPath;

    /// Page help topic.
    /// If page has a topic, then the Help button is displaued in the dialog
    /// and the onHelpRequested handler is called
    QString helpTopic;

    int margin = 6;
    int spacing = -1;
};

//------------------------------------------------------------------------------

class ConfigItem
{
public:
    ConfigItem(int pageId, const QString& title) : pageId(pageId), title(title) {}
    virtual ~ConfigItem() {}

    ConfigItem* withHint(const QString& h, bool wrap = true) { hint = h, wrapHint = wrap; return this; }

    int pageId;
    QString title;
    QString hint;
    bool wrapHint;
};

//------------------------------------------------------------------------------

class ConfigItemWidget : public ConfigItem
{
public:
    ConfigItemWidget(int pageId, QWidget *value) : ConfigItem(pageId, {}), value(value) {}

    QWidget *value;
};

//------------------------------------------------------------------------------

class ConfigItemSpace : public ConfigItem
{
public:
    ConfigItemSpace(int pageId, int value = 0) : ConfigItem(pageId, {}), value(value) {}

    int value;
};

//------------------------------------------------------------------------------

class ConfigItemBool : public ConfigItem
{
public:
    ConfigItemBool(int pageId, const QString& title, bool* value) : ConfigItem(pageId,  title), value(value) {}

    bool* value;
};

//------------------------------------------------------------------------------

class ConfigItemInt : public ConfigItem
{
public:
    ConfigItemInt(int pageId, const QString& title, int* value) : ConfigItem(pageId,  title), value(value) {}

    ConfigItem* withMinMax(int min, int max) { minValue = min, maxValue = max; return this; }

    int* value;
    std::optional<int> minValue, maxValue;
};

//------------------------------------------------------------------------------

class ConfigItemReal : public ConfigItem
{
public:
    ConfigItemReal(int pageId, const QString& title, double* value) : ConfigItem(pageId,  title), value(value) {}

    double* value;
};

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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
