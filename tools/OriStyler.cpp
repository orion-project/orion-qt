#include "OriStyler.h"

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>

namespace Ori {

Styler::Styler(QObject* parent) : QObject(parent)
{
}

Styler::Styler(const QString& style, QObject* parent) : QObject(parent)
{
    setCurrentStyle(style);
}

QStringList Styler::getStyles() const
{
    QStringList styles = QStyleFactory::keys();
    styles.sort();
    return styles;
}

QString Styler::currentStyle() const
{
    return qApp->style()->objectName();
}

void Styler::setCurrentStyle(const QString& style)
{
    qApp->setStyle(style);
}

} // namespace Ori
