#include "OriStylesMenu.h"
#include "../tools/OriStyler.h"

namespace Ori {
namespace Widgets {

StylesMenu::StylesMenu(QWidget *parent) : StylesMenu(0, parent)
{
    _styler = new Styler(this);
}

StylesMenu::StylesMenu(Styler *styler, QWidget *parent) : QMenu(parent), _styler(styler)
{
    setTitle(tr("&Styles", "Menu title"));

    connect(this, SIGNAL(aboutToShow()), this, SLOT(highlightCurrentStyle()));
}

void StylesMenu::populate()
{
    for (auto s: _styler->getStyles())
    {
        auto a = addAction(s, this, SLOT(applyStyle()));
        a->setCheckable(true);
    }
}

void StylesMenu::applyStyle()
{
    _styler->setCurrentStyle(qobject_cast<QAction*>(sender())->text());
}

void StylesMenu::highlightCurrentStyle()
{
    if (isEmpty()) populate();

    auto style = _styler->currentStyle();
    for (auto a: actions())
        a->setChecked(a->text().compare(style, Qt::CaseInsensitive) == 0);
}

} // namespace Widgets
} // namespace Ori
