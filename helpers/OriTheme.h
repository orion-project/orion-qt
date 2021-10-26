#ifndef ORI_THEME_H
#define ORI_THEME_H

#include <QString>

// This module supposes that the main application stylesheet file is app.qss
// Its devtime location is `$$_PRO_FILE_PWD_/src/app.qss`
// Its runtime location is `:/style/app` (in resources)

namespace Ori {
namespace Theme {

QString loadRawStyleSheet();
QString saveRawStyleSheet(const QString& text);
QString makeStyleSheet(const QString& rawStyleSheet);

} // namespace Theme
} // namespace Ori

#endif // ORI_THEME_H
