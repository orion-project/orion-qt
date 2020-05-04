#include "OriSvgView.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QSvgRenderer>

namespace Ori {
namespace Widgets {

SvgView* SvgView::makeStatic(const QString& path, QWidget *parent)
{
    auto v = new SvgView(parent);
    v->load(path);
    return v;
}

SvgView::SvgView(QWidget *parent) : SvgView(QString(), parent)
{
}

SvgView::SvgView(const QString& prefix, QWidget *parent) : QWidget(parent)
{
    this->prefix = prefix;
    renderer = new QSvgRenderer(this);
    connect(renderer, SIGNAL(repaintNeeded()), this, SLOT(update()));
}

void SvgView::load(const QString& resource)
{
    if (renderer->load(prefix + resource))
        updateAspect();
    else
        clear();
}

void SvgView::updateAspect()
{
    QSize sz = renderer->defaultSize();
    aspect = float(sz.height()) / sz.width();
}

void SvgView::clear()
{
    renderer->load(QByteArray(""));
}

void SvgView::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setViewport(0, 0, width(), height());

    // fill background
    QRect r = contentsRect();
    r.adjust(1, 1, -1, -1);
    p.fillRect(r, Qt::white);

    // draw styled frame
    QStyleOptionFrame opt;
    opt.init(this);
    opt.rect = contentsRect();
    opt.state |= QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);

    // graw svg image
    QRectF area;
    if (float(height())/width() < aspect)
    {
        float h = height()-4, w = h / aspect;
        area.setRect((width() - w) / 2, 2, w, h);
    }
    else
    {
        float w = width()-4, h = w * aspect;
        area.setRect(2, (height() - h) / 2, w, h);
    }
    renderer->render(&p, area);
}

QSize SvgView::sizeHint() const
{
    return QSize(220, 220);
}

} // namespace Widgets
} // namespace Ori
