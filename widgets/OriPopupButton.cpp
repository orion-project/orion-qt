#include "OriPopupButton.h"

#include <QMouseEvent>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOption>
#include <QPainter>

namespace Ori {
namespace Widgets {

////////////////////////////////////////////////////////////////////////////////
//                             PopupButton
////////////////////////////////////////////////////////////////////////////////

PopupButton::PopupButton(QWidget *parent) : QToolButton(parent), mPopup(0)
{
}

void PopupButton::setPopup(PopupGrid *value)
{
    mPopup = value;
    connect(mPopup, SIGNAL(closed()), this, SLOT(popupClosed()));
}

void PopupButton::mousePressEvent(QMouseEvent *event)
{
    QToolButton::mousePressEvent(event);
    if(event->button() == Qt::LeftButton && mPopup)
    {
        mPopup->justOpened = true;
        mPopup->show(mapToGlobal(rect().bottomLeft()));
    }
}

void PopupButton::popupClosed()
{
    setDown(false);
}

////////////////////////////////////////////////////////////////////////////////
//                                PopupGrid
////////////////////////////////////////////////////////////////////////////////

PopupGrid::PopupGrid(QWidget *parent) : QWidget(parent, Qt::Popup)
{
    setAttribute(Qt::WA_WindowPropagation);
    setMouseTracking(true);
}

void PopupGrid::addItem(const QString &item)
{
    mItems << item;
}

void PopupGrid::setItems(const QStringList &items, int colCount)
{
    mItems = items;
    mColCount = colCount;
    calcRowCount();
}

void PopupGrid::setColCount(int value)
{
    mColCount = value;
    calcRowCount();
}

void PopupGrid::calcRowCount()
{
    mRowCount = mItems.size() / mColCount + bool(mItems.size() % mColCount);
}

void PopupGrid::setCellSize(int value)
{
    mCellSize = value;
}

QSize PopupGrid::minimumSizeHint() const
{
    return QSize(mColCount * mCellSize + 2*marginH, mRowCount * mCellSize + 2*marginV);
}

QSize PopupGrid::sizeHint() const
{
    return minimumSizeHint();
}

void PopupGrid::show(const QPoint &at)
{
    move(at);
    hoverIndex = -1;
    pressIndex = -1;
    storeMetrics();
    setFixedSize(minimumSizeHint());
    QWidget::show();
}

void PopupGrid::storeMetrics()
{
    frameW = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, this);
    marginH = frameW + style()->pixelMetric(QStyle::PM_MenuHMargin, 0, this);
    marginV = frameW + style()->pixelMetric(QStyle::PM_MenuVMargin, 0, this);
}

void PopupGrid::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    QStyleOptionToolButton option;
    option.initFrom(this);
    option.palette = palette();
    option.iconSize = QSize(mCellSize, mCellSize);
    option.arrowType = Qt::NoArrow;
    option.subControls = QStyle::SC_ToolButton;
    option.activeSubControls = QStyle::SC_None;
    option.features = QStyleOptionToolButton::None;
    option.toolButtonStyle = Qt::ToolButtonIconOnly;
    option.font = font();

    //draw items
    QRect itemRect(marginH, marginV, mCellSize, mCellSize);
    for(int row = 0; row < mRowCount; ++row)
    {
        for(int col = 0; col < mColCount; ++col)
        {
            int index = row * mColCount + col;
            if(index < mItems.count() && event->rect().intersects(itemRect))
            {
                //set the clip region to be extra safe
                QRegion itemReg(itemRect);
                emptyArea -= itemReg;
                p.setClipRegion(itemReg);
                option.rect = itemRect;
                option.text = mItems[index];
                option.state = QStyle::State_Active | QStyle::State_Enabled;
                if(index == pressIndex)
                    option.state |= (pressIndex == hoverIndex)?
                                QStyle::State_Sunken : QStyle::State_Raised;
                if(index == hoverIndex && pressIndex < 0)
                    option.state |= QStyle::State_Raised;
                style()->drawComplexControl(QStyle::CC_ToolButton, &option, &p, this);
            }
            itemRect.translate(mCellSize, 0);
        }
        itemRect.moveTo(marginH, mCellSize*(row+1) + marginV);
    }

    //draw border
    if(frameW)
    {
        QRegion borderReg;
        borderReg += QRect(0, 0, frameW, height()); //left
        borderReg += QRect(width()-frameW, 0, frameW, height()); //right
        borderReg += QRect(0, 0, width(), frameW); //top
        borderReg += QRect(0, height()-frameW, width(), frameW); //bottom
        p.setClipRegion(borderReg);
        emptyArea -= borderReg;
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = frameW;
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
    }

    //finally the rest of the space
    p.setClipRegion(emptyArea);
    p.fillRect(rect(), palette().window());
}

void PopupGrid::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit closed();
}

void PopupGrid::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Return:
        if(pressIndex != -1)
            emit selected(mItems[pressIndex]);
        event->accept();
        close();
        return;

    case Qt::Key_Escape:
        event->accept();
        close();
        return;

    default:
        event->ignore();
    }
    QWidget::keyPressEvent(event);
}

void PopupGrid::mouseReleaseEvent(QMouseEvent *event)
{
    if(!hasFocus()) setFocus();
    if(!justOpened)
    {
        if(hoverIndex == pressIndex)
        {
            if(hoverIndex != -1)
                emit selected(mItems[pressIndex]);
            close();
        }
        else
        {
            pressIndex = -1;
            repaint();
        }
    }
    else
        justOpened = false;
    event->accept();
}

void PopupGrid::mousePressEvent(QMouseEvent *event)
{
    if(!hasFocus()) setFocus();
    event->accept();
    pressIndex = hoverIndex;
    repaint();
}

void PopupGrid::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    hoverIndex = indexAtPos(event->pos());
    repaint();
}

int PopupGrid::indexAtPos(const QPoint &pos)
{
    int x = pos.x(), y = pos.y();
    if(x < marginH || y < marginV ||
            (x >= width() - marginH) || (y >= height() - marginV)) return -1;
    x = (y-marginV) / mCellSize * mColCount + (x-marginH) / mCellSize;
    return (x < mItems.count())? x: -1;
}

} // namespace Widgets
} // namespace Ori
