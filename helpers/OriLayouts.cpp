#include "OriLayouts.h"

#include <QApplication>
#include <QGroupBox>
#include <QStyle>

namespace Ori {
namespace Layouts {

static qreal defSpacing(bool vert)
{
    return qApp->style()->pixelMetric(vert ?
        QStyle::PM_LayoutVerticalSpacing : QStyle::PM_LayoutHorizontalSpacing);
}

SpaceH::SpaceH(qreal factor)
{
    _mode = LayoutItemMode::Space;
    _space = qRound(defSpacing(false) * qAbs(factor));
}

SpaceV::SpaceV(qreal factor)
{
    _mode = LayoutItemMode::Space;
    _space = qRound(defSpacing(true) * qAbs(factor));
}

LayoutBox& LayoutBox::setDefSpacing(qreal factor)
{
    bool vert = qobject_cast<QVBoxLayout*>(_layout);
    boxLayout()->setSpacing(qRound(defSpacing(vert) * factor));
    return *this;
}

QGroupBox* LayoutBox::makeGroupBox(const QString& title)
{
    auto gb = new QGroupBox(title);
    gb->setLayout(_layout);
    return gb;
}

} // namespace Ori
} // namespace Layouts
