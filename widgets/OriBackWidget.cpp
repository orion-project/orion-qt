#include "OriBackWidget.h"

#include <QPainter>

namespace Ori {
namespace Widgets {

BackWidget::BackWidget(const QString& imagePath, QWidget* parent) : QWidget(parent)
{
    _background = QPixmap(imagePath);
    _alignment = Qt::AlignBottom | Qt::AlignRight;
}

BackWidget::BackWidget(const QString& imagePath, Qt::Alignment alignment, QWidget* parent) : BackWidget(imagePath, parent)
{
    _alignment = alignment;
}

void BackWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    int x = (_alignment & Qt::AlignLeft)? 0: width() - _background.width();
    int y = (_alignment & Qt::AlignTop)? 0: height() - _background.height();

    QPainter(this).drawPixmap(x, y, _background);
}

} // namespace Widgets
} // namespace Ori
