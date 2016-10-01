#include "OriActions.h"

namespace Ori {
namespace Widgets {

ExclusiveActionGroup::ExclusiveActionGroup(QObject *parent) : QActionGroup(parent)
{
    setExclusive(true);
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
}

void ExclusiveActionGroup::add(int id, const QString& icon, const QString& text)
{
    Option o;
    o.id = id;
    o.action = addAction(QIcon(icon), text);
    o.action->setCheckable(true);
    _options.append(o);

    if (_options.size() == 1)
        o.action->setChecked(true);
}

void ExclusiveActionGroup::setCheckedId(int id)
{
    for (const Option& o : _options)
        o.action->setChecked(o.id == id);
}

void ExclusiveActionGroup::triggered(QAction* action)
{
    for (const Option& o : _options)
        if (o.action == action)
        {
            emit checked(o.id);
            return;
        }
}

int ExclusiveActionGroup::checkedId()
{
    for (const Option& o : _options)
        if (o.action->isChecked())
            return o.id;
    if (_options.size() > 0)
        return _options[0].id;
    return -1;
}

} // namespace Widgets
} // namespace Ori
