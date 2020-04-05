#ifndef ORI_OPTIONS_GROUP_H
#define ORI_OPTIONS_GROUP_H

#include <QMap>
#include <QGroupBox>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

class OptionsGroup : public QGroupBox
{
    Q_OBJECT

public:
    explicit OptionsGroup(bool radio = false, QWidget *parent = nullptr);
    explicit OptionsGroup(const QString &title, bool radio = false, QWidget *parent = nullptr);
    explicit OptionsGroup(const QString &title, std::initializer_list<QString> options,
        bool radio = false, QWidget *parent = nullptr);

    int option();
    bool option(int id) const;
    void addOption(const QString &title);
    void addOption(int id, const QString &title);
    void addOptions(std::initializer_list<QString> options);
    void setOption(int id, bool value = true);

    void addControls(std::initializer_list<QObject*> controls);

signals:
    void optionChecked(int id);
    
public slots:
    void buttonClicked(bool checked);

private:
    bool _radio;
    QMap<int, QAbstractButton*> _options;
    QAbstractButton* getOption(int id) const;
};

} // namespace Widgets
} // namespace Gui

#endif // ORI_OPTIONS_GROUP_H
