#include "OriConfigDlg.h"

#include "helpers/OriLayouts.h"
#include "widgets/OriValueEdit.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
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
    
    void setEnabled(bool on) override
    {
        checkBox->setEnabled(on && !item->disabled);
    }

    ConfigItemBool* item;
    QCheckBox *checkBox = nullptr;
    QRadioButton *radioBtn = nullptr;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorRadio
//------------------------------------------------------------------------------

class ConfigItemEditorRadio : public ConfigItemEditor
{
public:
    ConfigItemEditorRadio(ConfigItemRadio* item): ConfigItemEditor(), item(item)
    {
        auto layout = new QHBoxLayout;
        for (const auto &it : std::as_const(item->items)) {
            auto but = new QRadioButton(it);
            layout->addWidget(but);
            buttons << but;
        }
        auto group = new QGroupBox(item->title);
        group->setLayout(layout);
        LayoutV({group, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        if (*item->value >= 0 && *item->value < buttons.size())
            buttons[*item->value]->setChecked(true);
    }

    void collect() override
    {
        for (int i = 0; i < buttons.size(); i++)
            if (buttons[i]->isChecked()) {
                *item->value = i;
                break;
            }
    }

    ConfigItemRadio* item;
    QList<QRadioButton*> buttons;
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
//                            ConfigItemEditorFile
//------------------------------------------------------------------------------

class ConfigItemEditorFile : public ConfigItemEditor
{
public:
    ConfigItemEditorFile(ConfigItemFile* item): ConfigItemEditor(), item(item)
    {
        control = new QLineEdit;
        connect(control, &QLineEdit::editingFinished, this, &ConfigItemEditorFile::updateStatus);
        status = new QLabel("<font color=red><b>" + qApp->tr("File does not exist") + "</b></font>");
        auto but = new QPushButton("•••");
        but->setFixedWidth(24);
        but->connect(but, &QPushButton::pressed, this, &ConfigItemEditorFile::selectFile);
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

    void selectFile()
    {
        QString file = QFileDialog::getOpenFileName(nullptr, QString(), control->text(), item->filter);
        if (!file.isEmpty()) {
            control->setText(file);
            updateStatus();
        }
    }

    void updateStatus()
    {
        status->setVisible(!QFile(control->text().trimmed()).exists());
    }

    ConfigItemFile* item;
    QLabel *status;
    QLineEdit* control;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorButton
//------------------------------------------------------------------------------

class ConfigItemEditorButton : public ConfigItemEditor
{
public:
    ConfigItemEditorButton(ConfigItemButton* item): ConfigItemEditor(), item(item)
    {
        button = new QPushButton(item->title);
        button->connect(button, &QPushButton::pressed, this, item->handler);
        LayoutV({button, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void setEnabled(bool on) override
    {
        button->setEnabled(on);
    }

    ConfigItemButton* item;
    QPushButton *button;
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
//                            ConfigItemEditorCustom
//------------------------------------------------------------------------------

class ConfigItemEditorCustom : public ConfigItemEditor
{
public:
    ConfigItemEditorCustom(ConfigItemCustom* item): ConfigItemEditor(), item(item)
    {
        LayoutV({item->title, item->editor, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        item->editor->populate();
    }

    void collect() override
    {
        item->editor->collect();
    }

    ConfigItemCustom *item;
};

//------------------------------------------------------------------------------
//                           ConfigItemEditorDropDown
//------------------------------------------------------------------------------

class ConfigItemEditorDropDown : public ConfigItemEditor
{
public:
    ConfigItemEditorDropDown(ConfigItemDropDown* item): ConfigItemEditor(), item(item)
    {
        control = new QComboBox;
        for (const auto &op : std::as_const(item->options))
            control->addItem(op.second, op.first);
        LayoutV({item->title, control, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        for (int i = 0; i < control->count(); i++)
            if (control->itemData(i).toInt() == *item->value) {
                control->setCurrentIndex(i);
                break;
            }
    }

    void collect() override
    {
        if (control->currentIndex() >= 0)
            *item->value = control->itemData(control->currentIndex()).toInt();
    }

    ConfigItemDropDown *item;
    QComboBox *control;
};


//------------------------------------------------------------------------------
//                           ConfigItemEditorIntPair
//------------------------------------------------------------------------------

class ConfigItemEditorIntPair : public ConfigItemEditor
{
public:
    ConfigItemEditorIntPair(ConfigItemIntPair* item): ConfigItemEditor(), item(item)
    {
        control1 = new QSpinBox;
        auto p = control1->sizePolicy();
        p.setHorizontalPolicy(QSizePolicy::Expanding);
        control1->setSizePolicy(p);
        if (item->minValue1.has_value())
            control1->setMinimum(item->minValue1.value());
        if (item->maxValue1.has_value())
            control1->setMaximum(item->maxValue1.value());

        control2 = new QSpinBox;
        control2->setSizePolicy(p);
        if (item->minValue2.has_value())
            control2->setMinimum(item->minValue2.value());
        if (item->maxValue2.has_value())
            control2->setMaximum(item->maxValue2.value());

        LayoutV({item->title, LayoutH(
            {item->title1, control1, SpaceH(2), item->title2, control2}
        ), hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
    }

    void populate() override
    {
        control1->setValue(*item->value1);
        control2->setValue(*item->value2);
    }

    void collect() override
    {
        *item->value1 = control1->value();
        *item->value2 = control2->value();
    }

    ConfigItemIntPair* item;
    QSpinBox *control1, *control2;
};

//------------------------------------------------------------------------------
//                            ConfigItemEditorEmpty
//------------------------------------------------------------------------------

class ConfigItemEditorEmpty : public ConfigItemEditor
{
public:
    ConfigItemEditorEmpty(ConfigItemEmpty* item): ConfigItemEditor()
    {
        LayoutV({item->title, hintLabel(item)}).setMargin(0).setSpacing(3).useFor(this);
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
    if (opts.windowModal)
        setWindowModality(Qt::WindowModal);

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
        if (auto it = dynamic_cast<ConfigItemInfo*>(item); it) {
            auto label = new QLabel(it->title);
            label->setWordWrap(true);
            w->mainLayout()->addWidget(label);
        } else if (auto it = dynamic_cast<ConfigItemBool*>(item); it) {
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
            if (boolEditor->checkBox) {
            #if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
                connect(boolEditor->checkBox, &QCheckBox::checkStateChanged, this, [this, item, &opts](Qt::CheckState state){
                    enableChildEditors(item, state == Qt::Checked);
                });
            #else
                connect(boolEditor->checkBox, &QCheckBox::stateChanged, this, [this, item, &opts](int state){
                    enableChildEditors(item, state == Qt::Checked);
                });
            #endif
            }
            editor = boolEditor;
        }
        else if (auto it = dynamic_cast<ConfigItemRadio*>(item); it)
            editor = new ConfigItemEditorRadio(it);
        else if (auto it = dynamic_cast<ConfigItemInt*>(item); it)
            editor = new ConfigItemEditorInt(it);
        else if (auto it = dynamic_cast<ConfigItemReal*>(item); it)
            editor = new ConfigItemEditorReal(it);
        else if (auto it = dynamic_cast<ConfigItemStr*>(item); it)
            editor = new ConfigItemEditorStr(it);
        else if (auto it = dynamic_cast<ConfigItemDir*>(item); it)
            editor = new ConfigItemEditorDir(it);
        else if (auto it = dynamic_cast<ConfigItemFile*>(item); it)
            editor = new ConfigItemEditorFile(it);
        else if (auto it = dynamic_cast<ConfigItemButton*>(item); it)
            editor = new ConfigItemEditorButton(it);
        else if (auto it = dynamic_cast<ConfigItemSection*>(item); it)
            editor = new ConfigItemEditorSection(it);
        else if (auto it = dynamic_cast<ConfigItemCustom*>(item); it)
            editor = new ConfigItemEditorCustom(it);
        else if (auto it = dynamic_cast<ConfigItemDropDown*>(item); it)
            editor = new ConfigItemEditorDropDown(it);
        else if (auto it = dynamic_cast<ConfigItemIntPair*>(item); it)
            editor = new ConfigItemEditorIntPair(it);
        else if (auto it = dynamic_cast<ConfigItemEmpty*>(item); it)
            editor = new ConfigItemEditorEmpty(it);
        if (editor)
        {
            if (int indent = getItemIndent(item, opts, 0); indent > 0) {
                auto l = LayoutH({editor}).setMargins(indent, 0, 0, 0);
                w->add(l.boxLayout());
            }
            else w->add(editor);
            
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

int ConfigDlg::getItemIndent(ConfigItem *item, const ConfigDlgOpts& opts, int indent) const
{
    if (item->parent)
        for (auto it : opts.items)
        {   
            if (it != item && it->pageId == item->pageId && it->valuePtr() && it->valuePtr() == item->parent)
                return indent + 16 + getItemIndent(it, opts, indent);
        }
    return indent;
}

void ConfigDlg::enableChildEditors(ConfigItem *item, bool on)
{
    for (auto itr = _editors.constBegin(); itr != _editors.constEnd(); itr++) {
        auto it = itr.key();
        auto editor = itr.value();
        if (it != item && it->pageId == item->pageId && item->valuePtr() && it->parent == item->valuePtr()) {
            editor->setEnabled(on);

            bool enabled = on;
            auto boolEditor = dynamic_cast<ConfigItemEditorBool*>(editor);
            if (boolEditor && boolEditor->checkBox) {
                enabled &= boolEditor->checkBox->isChecked();
            }
            enableChildEditors(it, enabled);
        }
    }
}

void ConfigDlg::populate()
{
    for (auto it = _editors.constBegin(); it != _editors.constEnd(); it++)
        it.value()->populate();
        
    for (auto it = _editors.constBegin(); it != _editors.constEnd(); it++) {
        auto boolEditor = dynamic_cast<ConfigItemEditorBool*>(it.value());
        if (boolEditor && boolEditor->checkBox) {
            enableChildEditors(it.key(), boolEditor->checkBox->isChecked());
        }
    }
}

bool ConfigDlg::collect()
{
    for (auto it = _editors.constBegin(); it != _editors.constEnd(); it++)
        it.value()->collect();
    return true;
}

} // namespace Dlg
} // namespace Ori
