#include "OriConfigDlg.h"

#include "helpers/OriLayouts.h"
#include "widgets/OriValueEdit.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QLabel>
#include <QSpinBox>
#include <QStyleHints>

using namespace Ori::Layouts;
using namespace Ori::Widgets;

namespace Ori {
namespace Dlg {

static QLabel* hintLabel(ConfigItem *it)
{
    if (it->hint.isEmpty())
        return nullptr;
    auto label = new QLabel(it->hint);
    label->setWordWrap(it->wrapHint);
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    label->setForegroundRole(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark ? QPalette::Light : QPalette::Mid);
#endif
    return label;
};

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

class ConfigItemEditorSpace : public ConfigItemEditor
{
public:
    ConfigItemEditorSpace(ConfigItemSpace* item): ConfigItemEditor(), item(item)
    {
        setFixedHeight(item->value);
    }

    void populate() override {}
    void collect() override {}

    ConfigItemSpace* item;
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
//                            ConfigItemEditorInt
//------------------------------------------------------------------------------

class ConfigItemEditorInt : public ConfigItemEditor
{
public:
    ConfigItemEditorInt(ConfigItemInt* item): ConfigItemEditor(), item(item)
    {
        control = new QSpinBox;
        if (item->minValue.has_value())
            control->setMinimum(item->minValue.value());
        if (item->maxValue.has_value())
            control->setMaximum(item->maxValue.value());
        LayoutV({item->title, control, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        control->setValue(*item->value);
    }

    void collect() override
    {
        *item->value = control->value();
    }

    ConfigItemInt* item;
    QSpinBox* control;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorReal
//------------------------------------------------------------------------------

class ConfigItemEditorReal : public ConfigItemEditor
{
public:
    ConfigItemEditorReal(ConfigItemReal* item): ConfigItemEditor(), item(item)
    {
        control = new ValueEdit;
        LayoutV({item->title, control, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        control->setValue(*item->value);
    }

    void collect() override
    {
        *item->value = control->value();
    }

    ConfigItemReal* item;
    ValueEdit* control;
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
        if (auto it = dynamic_cast<ConfigItemSpace*>(item); it)
            editor = new ConfigItemEditorSpace(it);
        else if (auto it = dynamic_cast<ConfigItemBool*>(item); it)
            editor = new ConfigItemEditorBool(it);
        else if (auto it = dynamic_cast<ConfigItemInt*>(item); it)
            editor = new ConfigItemEditorInt(it);
        else if (auto it = dynamic_cast<ConfigItemReal*>(item); it)
            editor = new ConfigItemEditorReal(it);
        if (editor)
        {
            w->add(editor);
            _editors.insert(item, editor);
        }
    }
    w->setContentsMargins(page.margin, page.margin, page.margin, page.margin);
    if (page.spacing >= 0)
        w->mainLayout()->setSpacing(page.spacing);
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
