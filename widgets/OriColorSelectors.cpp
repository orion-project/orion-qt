#include "OriColorSelectors.h"

#include <QColorDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <QWidgetAction>

// For Windows styles it looks better with BORDER_W=0
// and back buffer filled with pallete().button()
// But I'm more interested in the Fusion style which is the default for my apps
#define BORDER_W 1
#define SELECTED_FRAME_W 2

namespace Ori {
namespace Widgets {

//------------------------------------------------------------------------------
//                                 ColorGrid
//------------------------------------------------------------------------------

ColorGrid::ColorGrid(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void ColorGrid::setColors(const QList<QColor> &colors, int colCount)
{
    _colors = colors;
    if (colCount > 0)
        _colCount = colCount;
    calcRowCount();
    drawBackBuffer();
    update();
}

void ColorGrid::setColCount(int value)
{
    _colCount = qMax(1, value);
    calcRowCount();
    drawBackBuffer();
    update();
}

void ColorGrid::setCellSize(int value)
{
    _cellSize = qMax(5, value);
    drawBackBuffer();
    update();
}

void ColorGrid::setCellSpacing(int value)
{
    _cellSpacing = qMax(0, value);
    drawBackBuffer();
    update();
}

void ColorGrid::calcRowCount()
{
    _rowCount = _colors.size() / _colCount + int(bool(_colors.size() % _colCount));
}

QSize ColorGrid::minimumSizeHint() const
{
    return QSize((_cellSize + _cellSpacing)*_colCount + _cellSpacing + 2*BORDER_W,
                 (_cellSize + _cellSpacing)*_rowCount + _cellSpacing + 2*BORDER_W);
}

QSize ColorGrid::sizeHint() const
{
    return minimumSizeHint();
}

void ColorGrid::drawBackBuffer()
{
    _backBuffer = QPixmap(minimumSizeHint());
    QPainter p(&_backBuffer);
    p.setBrush(palette().base());
    p.setPen(QPen(palette().shadow(), BORDER_W));
    qreal fw = BORDER_W/2.0;
    p.drawRect(QRectF(_backBuffer.rect()).adjusted(fw, fw, -fw, -fw));

    int x, y = _cellSpacing + BORDER_W, idx = 0;
    for (int row = 0; row < _rowCount; row++, y += _cellSize+_cellSpacing)
    {
        x = _cellSpacing + BORDER_W;
        for (int col = 0; col < _colCount; col++, x += _cellSize+_cellSpacing)
        {
            if (idx == _colors.size())
                return;
            p.fillRect(x, y, _cellSize, _cellSize, _colors.at(idx));
            idx++;
        }
    }
}

void ColorGrid::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), palette().button());
    p.drawPixmap(0, 0, _backBuffer);

    if (_curIndex >= 0) {
        int x = (_curIndex % _colCount)*(_cellSize + _cellSpacing) + _cellSpacing + BORDER_W;
        int y = (_curIndex / _colCount)*(_cellSize + _cellSpacing) + _cellSpacing + BORDER_W;
        QPen pen(palette().highlight(), SELECTED_FRAME_W);
        pen.setJoinStyle(Qt::MiterJoin);
        p.setPen(pen);
        p.drawRect(
            qreal(x) - SELECTED_FRAME_W/2.0,
            qreal(y) - SELECTED_FRAME_W/2.0,
            qreal(_cellSize + SELECTED_FRAME_W),
            qreal(_cellSize + SELECTED_FRAME_W));
    }
}

void ColorGrid::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    qreal m = BORDER_W + _cellSpacing/2.0;
    auto pos = event->position() - QPointF(m, m);
    int col = pos.x() / (_cellSize + _cellSpacing);
    int row = pos.y() / (_cellSize + _cellSpacing);
    int index = _colCount * row + col;
    if (index >= _colors.size()) index = -1;
    if (index == _curIndex) return;
    _curIndex = index;
    update();

    if (_showTooltip)
        QToolTip::hideText();

    if (index < 0) return;

    const QColor& color = _colors.at(index);
    emit hovered(color);

    if (_showTooltip)
        QToolTip::showText(event->globalPosition().toPoint(), color.name(), this);
}

void ColorGrid::mousePressEvent(QMouseEvent*)
{
    if (!hasFocus())
        setFocus();
}

void ColorGrid::mouseReleaseEvent(QMouseEvent*)
{
    if (_curIndex >= 0)
        emit selected(_colors.at(_curIndex));
}

void ColorGrid::leaveEvent(QEvent*)
{
    if (_showTooltip)
        QToolTip::hideText();
    if (_curIndex >= 0) {
        _curIndex = -1;
        update();
    }
}

void ColorGrid::keyPressEvent(QKeyEvent* event)
{
    QToolTip::hideText();

    bool moved = false;
    int row = _curIndex / _colCount;
    int col = _curIndex % _colCount;

    switch (event->key()) {
    case Qt::Key_Right:
        col++;
        if (col == _colCount) col = 0;
        moved = true;
        break;

    case Qt::Key_Left:
        col--;
        if (col < 0) col = _colCount - 1;
        moved = true;
        break;

    case Qt::Key_Up:
        row--;
        if (row < 0) row = _rowCount - 1;
        if ((row * _colCount) + col >= _colors.size())
            row--;
        moved = true;
        break;

    case Qt::Key_Down:
        row = row + 1;
        if ((row * _colCount) + col >= _colors.size())
            row = 0;
        moved = true;
        break;

    case Qt::Key_Return:
        if (_curIndex >= 0)
            emit selected(_colors.at(_curIndex));
        event->accept();
        return;

    case Qt::Key_Escape:
        emit rejected();
        event->accept();
        return;

    default:
        event->ignore();
    }

    if (moved)
    {
        _curIndex = _curIndex >= 0 ? (_colCount*row + col) : 0;
        update();
        emit hovered(_colors.at(_curIndex));
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

//------------------------------------------------------------------------------
//                                 BaseColorButton
//------------------------------------------------------------------------------

BaseColorButton::BaseColorButton(QWidget* parent) : QToolButton(parent)
{
}

void BaseColorButton::setSelectedColor(const QColor& c)
{
    _color = c;

    if (_baseIcon.isNull())
    {
        QPixmap pixmap(iconSize());
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        p.setPen(palette().color(QPalette::Shadow));
        p.setBrush(_color);
        if (_iconRect.isEmpty())
            p.drawRect(pixmap.rect().adjusted(5, 5, -5, -5));
        else
            p.drawRect(_iconRect);
        setIcon(pixmap);
    }
    else
    {
        auto pixmap = _baseIcon.pixmap(iconSize());
        QPainter p(&pixmap);
        p.fillRect(_iconRect, _color);
        setIcon(pixmap);
    }
}

void BaseColorButton::setBaseIcon(const QIcon& icon)
{
    _baseIcon = icon;
    setSelectedColor(_color);
}

void BaseColorButton::setIconRect(const QRect& r)
{
    _iconRect = r;
    setSelectedColor(_color);
}

void BaseColorButton::showColorDialog()
{
    QColorDialog dlg;
    dlg.setOption(QColorDialog::DontUseNativeDialog, true);
    dlg.setCurrentColor(_color);
    if (dlg.exec())
    {
        setSelectedColor(dlg.selectedColor());
        emit colorSelected(_color);
    }
}

void BaseColorButton::resizeEvent(QResizeEvent*)
{
    setSelectedColor(_color);
}

//------------------------------------------------------------------------------
//                                 ColorButton
//------------------------------------------------------------------------------

ColorButton::ColorButton(QWidget* parent) : BaseColorButton(parent)
{
    connect(this, &QToolButton::clicked, this, &ColorButton::showColorDialog);
}

//------------------------------------------------------------------------------
//                               PopupColorButton
//------------------------------------------------------------------------------

PopupColorButton::PopupColorButton(QWidget* parent) : BaseColorButton(parent)
{
    setPopupMode(QToolButton::InstantPopup);

    _menu = new QMenu(this);
    setMenu(_menu);

    _grid = new ColorGrid(this);
    connect(_grid, &ColorGrid::selected, this, &PopupColorButton::onGridColorSelected);
    connect(_grid, &ColorGrid::rejected, _menu, &QMenu::hide);

    auto actnGrid = new QWidgetAction(this);
    actnGrid->setDefaultWidget(_grid);
    _menu->addAction(actnGrid);

    _actnColorDlg = _menu->addAction(tr("Choose Color..."));
    connect(_actnColorDlg, SIGNAL(triggered(bool)), this, SLOT(showColorDialog()));
}

void PopupColorButton::onGridColorSelected(const QColor& color)
{
    setSelectedColor(color);
    emit colorSelected(color);
    _menu->hide();
}

} // namespace Widgets
} // namespace Ori
