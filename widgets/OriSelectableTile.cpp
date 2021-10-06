#include "OriSelectableTile.h"

#include "../helpers/OriTools.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>

namespace Ori {
namespace Widgets {

//------------------------------------------------------------------------------
//                               IconCheckBox
//------------------------------------------------------------------------------

SelectableTile::SelectableTile(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);

    _iconLabel = new QLabel;
    _iconLabel->setAlignment(Qt::AlignHCenter);

    _titleLabel = new QLabel;
    _titleLabel->setAlignment(Qt::AlignHCenter);

    _content = new QWidget;
    _content->setAutoFillBackground(true);

    auto contentLayout = new QVBoxLayout(_content);
    contentLayout->setContentsMargins(6, 6, 6, 6);
    contentLayout->addWidget(_iconLabel);
    contentLayout->addWidget(_titleLabel);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(3, 3, 3, 3);
    mainLayout->addWidget(_content);

    updateColor();
}

void SelectableTile::setPixmap(const QPixmap& pixmap)
{
    _iconLabel->setPixmap(pixmap);
}

void SelectableTile::setTitle(const QString& title)
{
    _titleLabel->setText(title);
}

void SelectableTile::setTitleStyleSheet(const QString& styleSheet)
{
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
        updateColor();
}

void SelectableTile::focusOutEvent(QFocusEvent *event)
{
    QFrame::focusOutEvent(event);
    updateColor();
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

void SelectableTile::updateColor()
{
    QPalette p;
    auto baseColor = p.color(QPalette::Base);

    bool focused = hasFocus();

    if (!focused and !_selected)
    {
        p.setColor(QPalette::Window, baseColor);
        _content->setPalette(p);
        setPalette(p);
        return;
    }

    auto selectedColor = p.color(QPalette::Highlight);

    if (_selected)
    {
        p.setColor(QPalette::Window, selectedColor);
        setPalette(p);

        auto color = Ori::Color::blend(baseColor, selectedColor, focused ? 0.2 : 0.1);
        p.setColor(QPalette::Window, color);
        _content->setPalette(p);
        return;
    }

    // else focused
    auto color = Ori::Color::blend(baseColor, selectedColor, 0.1);
    p.setColor(QPalette::Window, color);
    _content->setPalette(p);
    setPalette(color);
}

void SelectableTile::setSelected(bool on)
{
    if (_selected == on) return;
    _selected = on;
    updateColor();
}

void SelectableTile::select(bool raiseEvent)
{
    _selected = true;
    updateColor();
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

} // namespace Widgets
} // namespace Ori
