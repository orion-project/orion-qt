#ifndef ORI_THEME_H
#define ORI_THEME_H

#include <QString>
#include <QColor>

/**
This module supposes that the main application stylesheet file is app.qss
Its devtime location is `$$_PRO_FILE_PWD_/src/app.qss`
Its runtime location is `:/style/app` (in resources)

Load app styles in main.cpp like:

```cpp
#include "helpers/OriTheme.h"

void main() {
    QApplication app(argc, argv);
    app.setStyleSheet(Ori::Theme::makeStyleSheet(Ori::Theme::loadRawStyleSheet()));

    MainWindow w;
    w.show();

    return app.exec();
}
```
*/

namespace Ori {
namespace Theme {

/**
Loads default application stylesheet from resources.
*/
QString loadRawStyleSheet();

/**
For dev mode only
To be used by a stylesheet editor available in app under special dev cmd line keys (--dev).
*/
QString saveRawStyleSheet(const QString& text);

/**
Parses and processes stylesheet string. Provides additional features for styles:

- Platform conditions for properties

```qss
#sql_console_result {
  font-family: Menlo,Monaco,Consolas,'Courier New',monospace;
  windows:font-size: 15px;
  linux:font-size: 14px;
  macos:font-size: 13px;
}
```

- Global variable substitution

```qss
$base-color: #dadbde;

QMainWindow {
  background-color: $base-color;
}

- Single line comments

```
*/
QString makeStyleSheet(const QString& rawStyleSheet);

#ifdef ORI_USE_STYLE_SHEETS
// Use style-sheets rather than palette for colorizing selected state of widgets.
// This is for cases when application is styled using QSS style-sheet.
// In this case
// a) palette still returns colors from current QStyle (e.g. Fusion),
//    not from style-sheet, and they don't match in most cases.
// b) assigning changed palette to a widget has no effect or partial effect
//    because palette colors get overriden by style-sheet colors.
// Since there is no easy way to get proper colors from current style-sheet,
// they should be set externally via setColors().

enum Color
{
    SelectionColor,
    PaperColor,
};

QColor color(Color kind);
void setColors(const QMap<Color, QColor> &colors);

#endif

} // namespace Theme
} // namespace Ori

#endif // ORI_THEME_H
