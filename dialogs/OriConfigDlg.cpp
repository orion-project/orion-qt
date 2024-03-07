#include "OriConfigDlg.h"

#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>

using namespace Ori::Layouts;

namespace Ori {
namespace Dlg {

//------------------------------------------------------------------------------
//                             ConfigItemEditor
//------------------------------------------------------------------------------

class ConfigItemEditor : public QWidget
{
public:
    ConfigItemEditor(): QWidget() {}
    virtual void populate() = 0;
    virtual void collect() = 0;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorBool
//------------------------------------------------------------------------------

class ConfigItemEditorBool : public ConfigItemEditor
{
public:
    ConfigItemEditorBool(ConfigItemBool* item): ConfigItemEditor(), item(item)
    {
        control = new QCheckBox(item->title);
        LayoutH({control}).setMargin(0).useFor(this);
    }

    void populate() override
    {
        control->setChecked(*item->value);
    }

    void collect() override
    {
        *item->value = control->isChecked();
    }

    ConfigItemBool* item;
    QCheckBox* control;
};

//------------------------------------------------------------------------------
//                              ConfigDlg
//------------------------------------------------------------------------------

bool ConfigDlg::edit(const ConfigDlgOpts& opts)
{
    ConfigDlg dlg(opts, qApp->activeWindow());
    return dlg.exec() == QDialog::Accepted;
}

ConfigDlg::ConfigDlg(const ConfigDlgOpts& opts, QWidget* parent) : BasicConfigDialog(parent)
{
    setObjectName(opts.objectName);
    setTitleAndIcon(opts.title, opts.iconPath);

    pageListIconSize = {opts.pageIconSize, opts.pageIconSize};

    QList<QWidget*> pageWidgets;
    for (const auto&  page : opts.pages)
        pageWidgets << makePage(page, opts);
    createPages(pageWidgets);

    if (opts.onHelpRequested)
        connect(this, &BasicConfigDialog::helpRequested, opts.onHelpRequested);

    if (opts.currentPageId >= 0)
        setCurrentPageId(opts.currentPageId);
}

ConfigDlg::~ConfigDlg()
{
    qDeleteAll(_editors.keyBegin(), _editors.keyEnd());
}

QWidget* ConfigDlg::makePage(const ConfigPage& page, const ConfigDlgOpts& opts)
{
    if (!page.helpTopic.isEmpty() && !opts.onHelpRequested)
        qWarning() << "ConfigDlg::makePage: page" << page.id <<
            "contains a help topic but no help request handler provided";

    auto w = new BasicConfigPage(page.id, page.title, page.iconPath, page.helpTopic);
    for (auto item : opts.items)
    {
        if (item->pageId != page.id) continue;

        ConfigItemEditor* editor = nullptr;
        if (auto it = dynamic_cast<ConfigItemBool*>(item); it)
            editor = new ConfigItemEditorBool(it);
        if (editor)
        {
            w->add(editor);
            _editors.insert(item, editor);
        }
    }
    w->add(w->stretch());
    return w;
}

void ConfigDlg::populate()
{
    for (auto it = _editors.constBegin(); it != _editors.constEnd(); it++)
        it.value()->populate();
}

bool ConfigDlg::collect()
{
    for (auto it = _editors.constBegin(); it != _editors.constEnd(); it++)
        it.value()->collect();
    return true;
}

} // namespace Dlg
} // namespace Ori
