#include "OriCharMap.h"

#include <QBoxLayout>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QToolTip>

namespace Ori {
namespace Widgets {

CharMap::CharMap(QWidget *parent) : QWidget(parent)
{
    lastKey = -1;

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    displayFont.setStyleStrategy(QFont::NoFontMerging);
    option.setAlignment(Qt::AlignCenter);

    connect(&scrollBar, SIGNAL(valueChanged(int)), this, SLOT(setTopRow(int)));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    layout->addWidget(&scrollBar);
    setLayout(layout);
}

QSize CharMap::sizeHint() const
{
    return QSize(640, 200);
}

void CharMap::setFont(const QFont &font)
{
    displayFont.setFamily(font.family());
    updateView(true);
}

void CharMap::setSize(const QString &fontSize)
{
    displayFont.setPointSize(fontSize.toInt());
    updateView();
}

void CharMap::setStyle(const QString &fontStyle)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    displayFont = QFontDatabase::font(displayFont.family(), fontStyle, displayFont.pointSize());
#else
    QFontDatabase fontDatabase;
    displayFont = fontDatabase.font(displayFont.family(), fontStyle, displayFont.pointSize());
#endif
    displayFont.setStyleStrategy(QFont::NoFontMerging);
    updateView(true);
}

void CharMap::updateView(bool updateChars)
{
    const int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    gridW = width() - 3*fw - style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    gridH = height() - 2*fw;

    QFontMetrics fm(displayFont);
    colCount = qRound(gridW / float(fm.averageCharWidth()) / 3.0);
    rowCount = qRound(gridH / float(fm.height()) / 1.2);
    if(updateChars)
    {
        chars.clear();
        for(int i = 1; i <= 65536; i++)
            if(fm.inFont(QChar(i)))
                chars.append(i);
    }
    totalRows = chars.count() / colCount + (chars.count() % colCount != 0);
    topRow = 0;
    scrollBar.setMaximum(totalRows-rowCount);
    scrollBar.setPageStep(rowCount-1);
    scrollBar.setValue(0);

    cellW = gridW / (float)colCount;
    cellH = gridH / (float)rowCount;
    update();
}

void CharMap::setTopRow(int value)
{
    if(topRow == value) return;
    if(value < 0) value = 0;
    if(value > totalRows - rowCount)
        value = totalRows - rowCount;
    topRow = value;
    scrollBar.setValue(topRow);
    update();
}

void CharMap::resizeEvent(QResizeEvent*)
{
    updateView();
}

void CharMap::mouseMoveEvent(QMouseEvent*)
{
    /*QPoint widgetPosition = mapFromGlobal(event->globalPos());
  uint key = (widgetPosition.y()/cellH)*columns + widgetPosition.x()/cellW;

  QString text = QString::fromLatin1("<p>Character: <span style=\"font-size: 24pt; font-family: %1\">").arg(displayFont.family())
               + QChar(key)
               + QString::fromLatin1("</span><p>Value: 0x")
               + QString::number(key, 16);
  QToolTip::showText(event->globalPos(), text, this);*/
}

void CharMap::mousePressEvent(QMouseEvent *event)
{
    /*  if (event->button() == Qt::LeftButton) {
         lastKey = (event->y()/cellH)*columns + event->x()/cellW;
         if (QChar(lastKey).category() != QChar::NoCategory)
             emit characterSelected(QString(QChar(lastKey)));
         update();
     }
     else*/
    QWidget::mousePressEvent(event);
}

void CharMap::wheelEvent(QWheelEvent *event)
{
    event->accept();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    topRow -= event->pixelDelta().manhattanLength() / 120;
#else
    topRow -= event->delta() / 120;
#endif
    if(topRow < 0) topRow = 0;
    if(topRow > totalRows - rowCount)
        topRow = totalRows - rowCount;
    scrollBar.setValue(topRow);
    update();
}

void CharMap::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    const int sw = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    const int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QRect r = rect();
    r.setWidth(r.width() - sw - fw);

    // draw styled frame
    QStyleOptionFrame opt;
    opt.rect = r;
    opt.state |= QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);

    r.adjust(fw, fw, -fw, -fw);
    p.setViewport(r);
    p.setWindow(QRect(0, 0, gridW, gridH));
    p.fillRect(p.window(), QBrush(Qt::white));

    // draw grid
    int pos;
    p.setPen(QPen(Qt::gray));
    for(int col = 1; col < colCount; col++)
    {
        pos = qRound(col * cellW);
        p.drawLine(pos, 0, pos, gridH);
    }
    for(int row = 1; row < rowCount; row++)
    {
        pos = qRound(row * cellH);
        p.drawLine(0, pos, gridW, pos);
    }

    // draw chars
    p.setPen(QPen(Qt::black));
    p.setFont(displayFont);
    for(int row = 0; row < rowCount; row++)
        for(int col = 0; col < colCount; col++)
        {
            int key = (row + topRow) * colCount + col;
            if(key >= chars.count()) break;
            //painter.setClipRect(column*cellW, row*cellH, cellW, cellH);
            //if (key == lastKey)
            //painter.fillRect(column*cellW+1, row*cellH+1, cellW, cellH, QBrush(Qt::red));
            //QRectF charRect(column*cellW, row*cellH, cellW, cellH);
            p.drawText(QRectF(col*cellW, row*cellH, cellW, cellH), QString(QChar(chars[key])), option);
        }
}

} // namespace Widgets
} // namespace Ori
