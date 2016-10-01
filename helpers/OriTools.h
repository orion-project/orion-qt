#ifndef ORI_OBJECTS_H
#define ORI_OBJECTS_H

#include <QColor>
#include <QString>

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

//--------------------------------------------------------------------------------------------------

namespace Ori {
namespace Tools {

QString getParamStr(const QUrl& url, const QString& name);
int getParamInt(const QUrl& url, const QString& name);

} // namespace Tools
} // namespace Ori

//--------------------------------------------------------------------------------------------------

namespace Ori {
namespace Color {

QString formatHtml(const QColor& c);
QString formatRgb(const QColor& c);
QColor blend(const QColor& color1, const QColor& color2, qreal r);
QColor random();

} // namespace Color
} // namespace Ori

//--------------------------------------------------------------------------------------------------

#endif // ORI_OBJECTS_H
