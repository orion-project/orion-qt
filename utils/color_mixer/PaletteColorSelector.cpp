#include "PaletteColorSelector.h"
#include "helpers/OriWidgets.h"

#define ADD_ITEM(item) addItem(# item, QPalette::item);

PaletteGroupCombo::PaletteGroupCombo(QWidget *parent) : QComboBox(parent)
{
    ADD_ITEM(Active)
    ADD_ITEM(Disabled)
    ADD_ITEM(Inactive)
    ADD_ITEM(NColorGroups)
    ADD_ITEM(Current)
    ADD_ITEM(All)
    ADD_ITEM(Normal)

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(itemSelected(int)));
}

void PaletteGroupCombo::itemSelected(int index)
{
    _selectedGroup = (QPalette::ColorGroup)itemData(index).toInt();
    emit groupSelected();
}

PaletteRoleCombo::PaletteRoleCombo(QWidget *parent) : QComboBox(parent)
{
    ADD_ITEM(WindowText)
    ADD_ITEM(Button)
    ADD_ITEM(Light)
    ADD_ITEM(Midlight)
    ADD_ITEM(Dark)
    ADD_ITEM(Mid)
    ADD_ITEM(Text)
    ADD_ITEM(BrightText)
    ADD_ITEM(ButtonText)
    ADD_ITEM(Base)
    ADD_ITEM(Window)
    ADD_ITEM(Shadow)
    ADD_ITEM(Highlight)
    ADD_ITEM(HighlightedText)
    ADD_ITEM(Link)
    ADD_ITEM(LinkVisited)
    ADD_ITEM(AlternateBase)
    ADD_ITEM(NoRole)
    ADD_ITEM(ToolTipBase)
    ADD_ITEM(ToolTipText)
    ADD_ITEM(NColorRoles)
    ADD_ITEM(Foreground)
    ADD_ITEM(Background)

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(itemSelected(int)));
}

void PaletteRoleCombo::itemSelected(int index)
{
    _selectedRole = (QPalette::ColorRole)itemData(index).toInt();
    emit roleSelected();
}

PaletteColorSelector::PaletteColorSelector(QWidget *parent) : QWidget(parent)
{
    Ori::Gui::layoutV(this, 0, -1, {
        _groupCombo = new PaletteGroupCombo,
        _roleCombo = new PaletteRoleCombo,
    });
    connect(_groupCombo, SIGNAL(groupSelected()), this, SLOT(makeColor()));
    connect(_roleCombo, SIGNAL(roleSelected()), this, SLOT(makeColor()));
}

void PaletteColorSelector::makeColor()
{
    QPalette p;
    _selectedColor = p.color(_groupCombo->selectedGroup(), _roleCombo->selectedRole());
    emit colorSelected(_selectedColor);
}
