#include "OriTools.h"

#include <QUrl>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif

//--------------------------------------------------------------------------------------------------

namespace Ori {
namespace Tools {

QString getParamStr(const QUrl& url, const QString& name)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QUrlQuery(url).queryItemValue(name);
#else
    return url.queryItemValue(name);
#endif
}

int getParamInt(const QUrl& url, const QString& name)
{
    return getParamStr(url, name).toInt();
}

} // namespace Tools
} // namespace Ori

//--------------------------------------------------------------------------------------------------

namespace Ori {
namespace Color {

QString formatHtml(const QColor& c)
{
    return QString("#%1%2%3")
            .arg(QString::number(c.red(), 16), 2, '0')
            .arg(QString::number(c.green(), 16), 2, '0')
            .arg(QString::number(c.blue(), 16), 2, '0');
}

QString formatRgb(const QColor& c)
{
    return QString("rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
}

QColor blend(const QColor& color1, const QColor& color2, qreal r)
{
    return QColor(color1.red() * (1-r) + color2.red()*r,
                  color1.green() * (1-r) + color2.green()*r,
                  color1.blue() * (1-r) + color2.blue()*r,
                  255);
}

QColor random()
{
    return QColor(qrand() % 255, qrand() % 255, qrand() % 255);
}

} // namespace Color
} // namespace Ori

//--------------------------------------------------------------------------------------------------
