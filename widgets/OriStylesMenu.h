#ifndef ORI_STYLES_MENU_H
#define ORI_STYLES_MENU_H

#include <QMenu>

namespace Ori {

class Styler;

namespace Widgets {

class StylesMenu : public QMenu
{
    Q_OBJECT

public:
    explicit StylesMenu(QWidget *parent = 0);
    explicit StylesMenu(Styler *styler, QWidget *parent = 0);

    Styler* styler() const { return _styler; }

private slots:
    void applyStyle();
    void highlightCurrentStyle();

private:
    Styler* _styler;

    void populate();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_STYLES_MENU_H
