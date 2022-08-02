#include "OriLabels.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

namespace Ori {
namespace Widgets {

//------------------------------------------------------------------------------
//                                  Label
//------------------------------------------------------------------------------

Label::Label(QWidget *parent, Qt::WindowFlags f) : Label(QString(), parent, f)
{
}

Label::Label(const QString &text, QWidget *parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
    _sizeHint = QLabel::sizeHint();
}

void Label::setSizeHint(QSize size)
{
    _sizeHint = size;
}

void Label::setSizeHint(int w, int h)
{
    _sizeHint = QSize(w, h);
}

void Label::setHorizontalSizeHint(int w)
{
    _sizeHint = QSize(w, _sizeHint.height());
}

void Label::setVerticalSizeHint(int h)
{
    _sizeHint = QSize(_sizeHint.width(), h);
}

void Label::setHorizontalSizePolicy(QSizePolicy::Policy policy)
{
    setSizePolicy(policy, sizePolicy().verticalPolicy());
}

void Label::setVerticalSizePolicy(QSizePolicy::Policy policy)
{
    setSizePolicy(sizePolicy().horizontalPolicy(), policy);
}

void Label::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
        emit clicked();
}

void Label::mouseDoubleClickEvent(QMouseEvent* event)
{
    QLabel::mouseDoubleClickEvent(event);

    if (event->button() == Qt::LeftButton)
        emit doubleClicked();
}

//------------------------------------------------------------------------------
//                             LabelSeparator
//------------------------------------------------------------------------------

LabelSeparator::LabelSeparator(QWidget *parent, Qt::WindowFlags f) : LabelSeparator(QString(), parent, f)
{
}

LabelSeparator::LabelSeparator(const QString &text, QWidget *parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

LabelSeparator::LabelSeparator(const QString &text, bool bold) : QLabel(text)
{
    if (bold)
    {
        QFont f = font();
        f.setBold(true);
        setFont(f);
    }
}

void LabelSeparator::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QRect cr = contentsRect();
    int m = margin();
    cr.adjust(m, m, -m, -m);
    QString text = QLabel::text();
    QPalette palette = QWidget::palette();
    if (!text.isEmpty())
    {
        QStyle *style = QWidget::style();
        int align = static_cast<int>(QStyle::visualAlignment(layoutDirection(), alignment()));
        QRect textRect = cr;
        textRect.adjust(textIndent, 0, -textIndent, 0);
        style->drawItemText(&p, textRect, align, palette, true, text, foregroundRole());
        textRect = style->itemTextRect(QFontMetrics(font()), textRect, align, true, text);
        p.setClipRegion(QRegion(cr) - textRect.adjusted(-4, 0, 4, 0));
    }
    int y = (cr.bottom() + cr.top()) / 2;
    p.setPen(palette.color(QPalette::Mid));
    p.drawLine(cr.left(), y, cr.right(), y);
    if (!flat) {
        p.setPen(palette.color(QPalette::Midlight));
        p.drawLine(cr.left(), y+1, cr.right(), y+1);
    }
}

//------------------------------------------------------------------------------
//                              ImagedLabel
//------------------------------------------------------------------------------

ImagedLabel::ImagedLabel(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    _icon = new QLabel;
    _text = new QLabel;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(3);
    layout->addWidget(_icon);
    layout->addWidget(_text);
    setLayout(layout);
}

void ImagedLabel::setContent(const QString& text, const QString& imagePath)
{
    _text->setText(text);
    if (imagePath.isEmpty())
        _icon->clear();
    else _icon->setPixmap(QIcon(imagePath).pixmap(16, 16));
}

void ImagedLabel::clear()
{
    _icon->clear();
    _text->clear();
}

} // namespace Widgets
} // namespace Ori
