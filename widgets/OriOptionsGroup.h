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
    void addOption(const QString &title, const QString &hint = QString());
    void addOption(int id, const QString &title, const QString &hint = QString());
    void addOptions(std::initializer_list<QString> options);
    void setOption(int id, bool value = true);

    void addControls(std::initializer_list<QObject*> controls);

    QString hintFormat;

signals:
    void optionChecked(int id);
    
public slots:
    void buttonClicked(bool checked);

private:
    bool _radio;
    QMap<int, QAbstractButton*> _options;
    QAbstractButton* getOption(int id) const;
};

class OptionsGroupV2 : public QGroupBox
{
    Q_OBJECT

public:
    explicit OptionsGroupV2(bool radio = false, QWidget *parent = nullptr);
    explicit OptionsGroupV2(const QString &title, bool radio = false, QWidget *parent = nullptr);
    explicit OptionsGroupV2(const QString &title, const QVector<QPair<QString, QString>>& options,
        bool radio = false, QWidget *parent = nullptr);

    QString option() const;
    bool option(const QString& id) const;
    void addOption(const QString& id, const QString &title);
    void addOptions(const QVector<QPair<QString, QString>>& options);
    void setOption(const QString& id, bool value = true);

    void addControls(std::initializer_list<QObject*> controls);

signals:
    void optionChecked(const QString& id);

private slots:
    void buttonClicked(bool checked);

private:
    bool _radio;
    QMap<QString, QAbstractButton*> _options;
    QAbstractButton* getButton(const QString& id) const;
};

} // namespace Widgets
} // namespace Gui

#endif // ORI_OPTIONS_GROUP_H
