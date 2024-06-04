#include "OriConfigDlg.h"

#include "helpers/OriLayouts.h"
#include "widgets/OriValueEdit.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
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
    virtual void populate() {}
    virtual void collect() {}
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorBool
//------------------------------------------------------------------------------

class ConfigItemEditorBool : public ConfigItemEditor
{
public:
    ConfigItemEditorBool(ConfigItemBool* item): ConfigItemEditor(), item(item)
    {
        QWidget *control;
        if (!item->radioGroupId.isEmpty()) {
            control = radioBtn = new QRadioButton(item->title);
            radioBtn->setDisabled(item->disabled);
        } else {
            control = checkBox = new QCheckBox(item->title);
            checkBox->setDisabled(item->disabled);
        }
        LayoutV({control, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        if (checkBox)
            checkBox->setChecked(*item->value);
        else radioBtn->setChecked(*item->value);
    }

    void collect() override
    {
        if (checkBox)
            *item->value = checkBox->isChecked();
        else *item->value = radioBtn->isChecked();
    }

    ConfigItemBool* item;
    QCheckBox *checkBox = nullptr;
    QRadioButton *radioBtn = nullptr;
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
//                            ConfigItemEditorStr
//------------------------------------------------------------------------------

class ConfigItemEditorStr : public ConfigItemEditor
{
public:
    ConfigItemEditorStr(ConfigItemStr* item): ConfigItemEditor(), item(item)
    {
        control = new QLineEdit;
        control->setReadOnly(item->readOnly);
        if (item->align.has_value())
            control->setAlignment(item->align.value());
        LayoutV({item->title, control, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        control->setText(*item->value);
    }

    void collect() override
    {
        if (!control->isReadOnly())
            *item->value = control->text();
    }

    ConfigItemStr* item;
    QLineEdit* control;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorDir
//------------------------------------------------------------------------------

class ConfigItemEditorDir : public ConfigItemEditor
{
public:
    ConfigItemEditorDir(ConfigItemDir* item): ConfigItemEditor(), item(item)
    {
        control = new QLineEdit;
        connect(control, &QLineEdit::editingFinished, this, &ConfigItemEditorDir::updateStatus);
        status = new QLabel("<font color=red><b>" + qApp->tr("Directory does not exist") + "</b></font>");
        auto but = new QPushButton("•••");
        but->setFixedWidth(24);
        but->connect(but, &QPushButton::pressed, this, &ConfigItemEditorDir::selectDir);
        LayoutV({LayoutH({item->title, but}), control, status, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        control->setText(item->value->trimmed());
        updateStatus();
    }

    void collect() override
    {
        *item->value = control->text().trimmed();
    }

    void selectDir()
    {
        QString dir = QFileDialog::getExistingDirectory(nullptr, QString(), control->text());
        if (!dir.isEmpty()) {
            control->setText(dir);
            updateStatus();
        }
    }

    void updateStatus()
    {
        status->setVisible(!QDir(control->text().trimmed()).exists());
    }

    ConfigItemDir* item;
    QLabel *status;
    QLineEdit* control;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorSection
//------------------------------------------------------------------------------

class ConfigItemEditorSection : public ConfigItemEditor
{
public:
    ConfigItemEditorSection(ConfigItemSection* item): ConfigItemEditor()
    {
        auto label = new QLabel(item->title);
        LayoutV({label, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
        auto font = label->font();
        font.setPointSizeF(font.pointSizeF() * 1.2);
        font.setBold(true);
        label->setFont(font);
    }
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
        if (auto it = dynamic_cast<ConfigItemWidget*>(item); it)
            w->mainLayout()->addWidget(it->value);
        if (auto it = dynamic_cast<ConfigItemSpace*>(item); it)
            w->mainLayout()->addSpacing(it->value);
        else if (auto it = dynamic_cast<ConfigItemBool*>(item); it) {
            auto boolEditor = new ConfigItemEditorBool(it);
            if (!it->radioGroupId.isEmpty()) {
                for (auto other = _editors.constBegin(); other != _editors.constEnd(); other++) {
                    if (auto otherBool = dynamic_cast<ConfigItemBool*>(other.key()); otherBool) {
                        if (otherBool->radioGroupId == it->radioGroupId && otherBool->pageId != it->pageId) {
                            qWarning()
                                << "Radio item" << it->title << "is on page" << it->pageId
                                << "There is another radio item" << otherBool->title
                                << "with the same group id" << it->radioGroupId
                                << "but on a different page" << otherBool->pageId;
                        }
                    }
                }
                if (!_radioGroups.contains(it->radioGroupId))
                    _radioGroups[it->radioGroupId] = new QButtonGroup(this);
                _radioGroups[it->radioGroupId]->addButton(boolEditor->radioBtn);
            }
            editor = boolEditor;
        }
        else if (auto it = dynamic_cast<ConfigItemInt*>(item); it)
            editor = new ConfigItemEditorInt(it);
        else if (auto it = dynamic_cast<ConfigItemReal*>(item); it)
            editor = new ConfigItemEditorReal(it);
        else if (auto it = dynamic_cast<ConfigItemStr*>(item); it)
            editor = new ConfigItemEditorStr(it);
        else if (auto it = dynamic_cast<ConfigItemDir*>(item); it)
            editor = new ConfigItemEditorDir(it);
        else if (auto it = dynamic_cast<ConfigItemSection*>(item); it)
            editor = new ConfigItemEditorSection(it);
        if (editor)
        {
            w->add(editor);
            _editors.insert(item, editor);
        }
    }
    w->setLongTitle(page.longTitle);
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
