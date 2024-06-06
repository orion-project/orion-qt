#include "OriSelectableTile.h"

#include "../helpers/OriTools.h"
#ifdef ORI_USE_STYLE_SHEETS
#include "../helpers/OriTheme.h"
#endif

#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>

namespace Ori::Widgets {

//------------------------------------------------------------------------------
//                           SelectableTileContentDefault
//------------------------------------------------------------------------------

SelectableTileContent::SelectableTileContent() : QFrame()
{
    setProperty("ori_role", "selectable_tile_content");
}

//------------------------------------------------------------------------------
//                           SelectableTileContentDefault
//------------------------------------------------------------------------------

SelectableTileContentDefault::SelectableTileContentDefault() : SelectableTileContent()
{
    setAutoFillBackground(true);
}

void SelectableTileContentDefault::updateState(bool focused, bool selected)
{
#ifdef ORI_USE_STYLE_SHEETS
    auto baseColor = Ori::Theme::color(Ori::Theme::PaperColor);
    auto selectedColor = Ori::Theme::color(Ori::Theme::SelectionColor);
#else
    QPalette p;
    auto baseColor = p.color(QPalette::Base);
    auto selectedColor = p.color(QPalette::Highlight);
#endif
    QColor color = (!focused and !selected) ? baseColor :
        Ori::Color::blend(baseColor, selectedColor, selected && focused ? 0.2 : 0.1);
#ifdef ORI_USE_STYLE_SHEETS
    setStyleSheet(QStringLiteral("background:%1").arg(color.name()));
#else
    p.setColor(QPalette::Window, color);
    setPalette(p);
#endif
}

//------------------------------------------------------------------------------
//                               SelectableTile
//------------------------------------------------------------------------------

SelectableTile::SelectableTile(bool makeDefaultLabels, QWidget *parent)
    : SelectableTile(new SelectableTileContentDefault(), makeDefaultLabels, parent)
{
}

SelectableTile::SelectableTile(SelectableTileContent *content, bool makeDefaultLabels, QWidget *parent)
    : QFrame(parent), _content(content)
{
    setProperty("ori_role", "selectable_tile");
    setFrameShape(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);

    if (makeDefaultLabels)
    {
        _iconLabel = new QLabel;
        _iconLabel->setAlignment(Qt::AlignHCenter);

        _titleLabel = new QLabel;
        _titleLabel->setAlignment(Qt::AlignHCenter);
        _titleLabel->setProperty("ori_role", "selectable_tile_title");

        auto contentLayout = new QVBoxLayout(_content);
        contentLayout->setContentsMargins(6, 6, 6, 6);
        contentLayout->addWidget(_iconLabel);
        contentLayout->addWidget(_titleLabel);
    }
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(3, 3, 3, 3);
    mainLayout->addWidget(_content);

    updateState();
}

void SelectableTile::setPixmap(const QPixmap& pixmap)
{
    if (_iconLabel)
        _iconLabel->setPixmap(pixmap);
}

void SelectableTile::setTitle(const QString& title)
{
    if (_titleLabel)
        _titleLabel->setText(title);
}

void SelectableTile::setTitleStyleSheet(const QString& styleSheet)
{
    if (_titleLabel)
        _titleLabel->setStyleSheet(styleSheet);
}

void SelectableTile::setData(const QVariant& data)
{
    _data = data;
}

void SelectableTile::focusInEvent(QFocusEvent *event)
{
    QFrame::focusInEvent(event);
    if (selectionFollowsFocus)
        select(true);
    else
        updateState();
}

void SelectableTile::focusOutEvent(QFocusEvent *event)
{
    QFrame::focusOutEvent(event);
    updateState();
}

void SelectableTile::keyPressEvent(QKeyEvent* event)
{
    QFrame::keyPressEvent(event);
    switch (event->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        emit goingSelectPrev();
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        emit goingSelectNext();
        break;
    case Qt::Key_Space:
    case Qt::Key_Return:
        select(true);
        break;
    }
}

void SelectableTile::mousePressEvent(QMouseEvent *event)
{
    QFrame::mousePressEvent(event);
    select(true);
}

void SelectableTile::mouseDoubleClickEvent(QMouseEvent *event)
{
    QFrame::mouseDoubleClickEvent(event);
    emit doubleClicked();
}

void SelectableTile::updateState()
{
    bool focused = hasFocus();
#ifdef ORI_USE_STYLE_SHEETS
    auto baseColor = Ori::Theme::color(Ori::Theme::PaperColor);
    auto selectedColor = Ori::Theme::color(Ori::Theme::SelectionColor);
#else
    QPalette p;
    auto baseColor = p.color(QPalette::Base);
    auto selectedColor = p.color(QPalette::Highlight);
#endif
    QColor color = (!focused and !_selected) ? baseColor :
        _selected ? selectedColor : Ori::Color::blend(baseColor, selectedColor, 0.1);
#ifdef ORI_USE_STYLE_SHEETS
    if (_selected)
        setStyleSheet(QStringLiteral("background:%1;border-color:%1").arg(color.name()));
    else
        setStyleSheet(QStringLiteral("background:%1").arg(color.name()));
#else
    p.setColor(QPalette::Window, color);
    setPalette(p);
#endif
    _content->updateState(focused, _selected);
}

void SelectableTile::setSelected(bool on)
{
    if (_selected == on) return;
    _selected = on;
    updateState();
}

void SelectableTile::select(bool raiseEvent)
{
    _selected = true;
    updateState();
    if (raiseEvent)
        emit selected();
}

//------------------------------------------------------------------------------
//                            IconCheckBoxGroup
//------------------------------------------------------------------------------

SelectableTileRadioGroup::SelectableTileRadioGroup(QObject *parent) : QObject(parent)
{
}

void SelectableTileRadioGroup::addTile(SelectableTile* tile)
{
    _tiles.append(tile);
    connect(tile, &SelectableTile::selected, this, &SelectableTileRadioGroup::tileSelected);
    connect(tile, &SelectableTile::goingSelectNext, this, &SelectableTileRadioGroup::selectNextTile);
    connect(tile, &SelectableTile::goingSelectPrev, this, &SelectableTileRadioGroup::selectPrevTile);
    connect(tile, &SelectableTile::doubleClicked, this, &SelectableTileRadioGroup::tileDoubleClicked);
}

void SelectableTileRadioGroup::selectNextTile()
{
    if (!selectionFollowsArrows) return;

    int count = _tiles.size();
    if (count < 2) return;
    auto tile = sender();
    int next = -1;
    for (int i = 0; i < count; i++)
        if (_tiles.at(i) == tile)
        {
            next = i+1;
            if (next > count-1) next = 0;
            break;
        }
    for (int i = 0; i < count; i++)
        _tiles.at(i)->setSelected(i == next);
    if (next >= 0)
        _tiles.at(next)->setFocus();
}

void SelectableTileRadioGroup::selectPrevTile()
{
    if (!selectionFollowsArrows) return;

    int count = _tiles.size();
    if (count < 2) return;
    auto tile = sender();
    int prev = -1;
    for (int i = 0; i < count; i++)
        if (_tiles.at(i) == tile)
        {
            prev = i-1;
            if (prev < 0) prev = count-1;
            break;
        }
    for (int i = 0; i < count; i++)
        _tiles.at(i)->setSelected(i == prev);
    if (prev >= 0)
        _tiles.at(prev)->setFocus();
}

void SelectableTileRadioGroup::tileSelected()
{
    auto tile = qobject_cast<SelectableTile*>(sender());
    if (!tile) return;
    for (int i = 0; i < _tiles.size(); i++)
        if (_tiles.at(i) != tile)
            _tiles.at(i)->setSelected(false);
    emit dataSelected(tile->data());
}

QVariant SelectableTileRadioGroup::selectedData() const
{
    for (auto tile : _tiles)
        if (tile->isSelected())
            return tile->data();
    return QVariant();
}

void SelectableTileRadioGroup::selectData(const QVariant& data)
{
    for (int i = 0; i < _tiles.size(); i++)
        _tiles.at(i)->setSelected(_tiles.at(i)->data() == data);
}


void SelectableTileRadioGroup::tileDoubleClicked()
{
    auto tile = qobject_cast<SelectableTile*>(sender());
    if (!tile) return;
    emit doubleClicked(tile->data());
}

} // namespace Ori::Widgets
