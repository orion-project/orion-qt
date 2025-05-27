#ifndef ORI_CONFIG_DLG_H
#define ORI_CONFIG_DLG_H

#include "OriBasicConfigDlg.h"

#include <optional>

#include <QMap>

class QButtonGroup;

namespace Ori {
namespace Dlg {

struct ConfigPage
{
    ConfigPage(int id, const QString& title, const QString& icon = QString()) : id(id), title(title), iconPath(icon) {}

    ConfigPage& withLongTitle(const QString &title) { longTitle = title; return *this; }
    ConfigPage& withHelpTopic(const QString& topic) { helpTopic = topic; return *this; }
    ConfigPage& withMargins(int m) { margin = m; return *this; }
    ConfigPage& withSpacing(int s) { spacing = s; return *this; }

    int id;
    QString title;
    QString longTitle;
    QString iconPath;

    /// Page help topic.
    /// If page has a topic, then the Help button is displaued in the dialog
    /// and the onHelpRequested handler is called
    QString helpTopic;

    int margin = 6;
    int spacing = -1;
};

//------------------------------------------------------------------------------

class ConfigItemEditor : public QWidget
{
public:
    ConfigItemEditor(): QWidget() {}
    virtual void populate() {}
    virtual void collect() {}
    virtual void setEnabled(bool) {}
};

//------------------------------------------------------------------------------

class ConfigItem
{
public:
    ConfigItem(int pageId, const QString& title) : pageId(pageId), title(title) {}
    virtual ~ConfigItem() {}

    ConfigItem* withHint(const QString& h, bool wrap = true) { hint = h, wrapHint = wrap; return this; }
    ConfigItem* withParent(void *value) { parent = value; return this; }
    
    virtual void* valuePtr() const { return nullptr; }

    int pageId;
    QString title;
    QString hint;
    bool wrapHint = true;
    void* parent = nullptr;
};

//------------------------------------------------------------------------------

class ConfigItemWidget : public ConfigItem
{
public:
    ConfigItemWidget(int pageId, QWidget *value) : ConfigItem(pageId, {}), value(value) {}

    void* valuePtr() const override { return value; }

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

    ConfigItemBool* setDisabled(bool disab) { disabled = disab; return this; }
    ConfigItemBool* withRadioGroup(const QString &groupId) { radioGroupId = groupId; return this; }

    void* valuePtr() const override { return value; }

    bool* value;
    bool disabled = false;
    QString radioGroupId;
};

//------------------------------------------------------------------------------

class ConfigItemRadio : public ConfigItem
{
public:
    ConfigItemRadio(int pageId, const QString& title, const QStringList &items, int *value) : ConfigItem(pageId, title), items(items), value(value) {}

    void* valuePtr() const override { return value; }

    QStringList items;
    int *value;
};

//------------------------------------------------------------------------------

class ConfigItemInt : public ConfigItem
{
public:
    ConfigItemInt(int pageId, const QString& title, int* value) : ConfigItem(pageId,  title), value(value) {}

    ConfigItem* withMinMax(int min, int max) { minValue = min, maxValue = max; return this; }

    void* valuePtr() const override { return value; }
    
    int* value;
    std::optional<int> minValue, maxValue;
};

//------------------------------------------------------------------------------

class ConfigItemReal : public ConfigItem
{
public:
    ConfigItemReal(int pageId, const QString& title, double* value) : ConfigItem(pageId,  title), value(value) {}

    void* valuePtr() const override { return value; }

    double* value;
};

//------------------------------------------------------------------------------

class ConfigItemStr : public ConfigItem
{
public:
    ConfigItemStr(int pageId, const QString& title, QString* value) : ConfigItem(pageId,  title), value(value) {}

    ConfigItemStr* withAlignment(Qt::Alignment a) { align = a; return this; }
    ConfigItemStr* withReadOnly() { readOnly = true; return this; }

    void* valuePtr() const override { return value; }

    QString* value;
    std::optional<Qt::Alignment> align;
    bool readOnly = false;
};

//------------------------------------------------------------------------------

class ConfigItemDir : public ConfigItem
{
public:
    ConfigItemDir(int pageId, const QString& title, QString* value) : ConfigItem(pageId,  title), value(value) {}

    void* valuePtr() const override { return value; }

    QString* value;
};

//------------------------------------------------------------------------------

class ConfigItemFile : public ConfigItem
{
public:
    ConfigItemFile(int pageId, const QString& title, QString* value) : ConfigItem(pageId,  title), value(value) {}

    ConfigItemFile* withFilter(const QString& f) { filter = f; return this; }

    void* valuePtr() const override { return value; }

    QString* value;
    QString filter;
};

//------------------------------------------------------------------------------

class ConfigItemSection : public ConfigItem
{
public:
    ConfigItemSection(int pageId, const QString& title) : ConfigItem(pageId,  title) {}
};

//------------------------------------------------------------------------------

class ConfigItemCustom : public ConfigItem
{
public:
    ConfigItemCustom(int pageId, const QString& title, ConfigItemEditor *editor) : ConfigItem(pageId, title), editor(editor) {}

    ConfigItemEditor *editor;
};

//------------------------------------------------------------------------------

class ConfigItemDropDown : public ConfigItem
{
public:
    ConfigItemDropDown(int pageId, const QString& title, int* value) : ConfigItem(pageId,  title), value(value) {}

    ConfigItemDropDown* withOption(int id, const QString& text) { options << QPair<int, QString>{id, text}; return this; }

    void* valuePtr() const override { return value; }
    
    int* value;
    QList<QPair<int, QString>> options;
};

//------------------------------------------------------------------------------

class ConfigItemIntPair : public ConfigItem
{
public:
    ConfigItemIntPair(int pageId, const QString& title, const QString& title1, int* value1, const QString& title2, int* value2)
        : ConfigItem(pageId,  title), title1(title1), title2(title2), value1(value1), value2(value2) {}

    ConfigItem* withMinMax(int min, int max) { minValue1 = minValue2 = min, maxValue1 = maxValue2 = max; return this; }
    ConfigItem* withMinMax1(int min, int max) { minValue1 = min, maxValue1 = max; return this; }
    ConfigItem* withMinMax2(int min, int max) { minValue2 = min, maxValue2 = max; return this; }

    void* valuePtr() const override { return value1; }

    QString title1, title2;
    int *value1, *value2;
    std::optional<int> minValue1, maxValue1;
    std::optional<int> minValue2, maxValue2;
};

//------------------------------------------------------------------------------

class ConfigItemEmpty : public ConfigItem
{
public:
    ConfigItemEmpty(int pageId, const QString& title) : ConfigItem(pageId, title) {}
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
    int currentPageId = -1;

    /// Block only parent window. It's useful when dialog shows in-app help window.
    /// Set to false to make the dialog app modal (block all app windows).
    bool windowModal = true;

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
    QMap<QString, QButtonGroup*> _radioGroups;

    QWidget* makePage(const ConfigPage& page, const ConfigDlgOpts& opts);
    int getItemIndent(ConfigItem *item, const ConfigDlgOpts &opts, int indent) const;
    void enableChildItems(ConfigItem *item, const ConfigDlgOpts& opts, bool on);
    void enableChildEditors(ConfigItem *item, bool on);
};

} // namespace Dlg
} // namespace Ori

#endif // ORI_CONFIG_DLG_H
