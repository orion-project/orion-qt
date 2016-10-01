#ifndef ORI_ACTIONS_H
#define ORI_ACTIONS_H

#include <QActionGroup>

namespace Ori {
namespace Widgets {

class ExclusiveActionGroup : public QActionGroup
{
    Q_OBJECT

public:
    explicit ExclusiveActionGroup(QObject *parent = 0);

    void add(int id, const QString& icon, const QString& text);

    int checkedId();
    void setCheckedId(int id);

signals:
    void checked(int id);

private:
    struct Option { int id; QAction* action; };
    QVector<Option> _options;

private slots:
    void triggered(QAction* action);
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_ACTIONS_H
