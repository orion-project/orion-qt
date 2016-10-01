#ifndef ORI_MRU_MENU_H
#define ORI_MRU_MENU_H

#include <QMap>
#include <QMenu>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QSettings;
class QVBoxLayout;
QT_END_NAMESPACE

namespace Ori {
class MruList;
}

namespace Ori {
namespace Widgets {

////////////////////////////////////////////////////////////////////////////////

class MruMenu : public QMenu
{
    Q_OBJECT

public:
    MruMenu(const QString& title, MruList *mru, QWidget *parent = 0);
    MruMenu(MruList *mru, QWidget *parent = 0);

private:
    QPointer<MruList> _mru;

private slots:
    void populate();
};

////////////////////////////////////////////////////////////////////////////////

class MruMenuPart : public QObject
{
    Q_OBJECT

public:
    MruMenuPart(MruList *mru, QMenu *menu, QAction *placeholder, QWidget *parent = 0);

private:
    QPointer<MruList> _mru;
    QPointer<QMenu> _menu;
    QPointer<QAction> _placeholder;
    QAction *_separator;

private slots:
    void populate();
};

////////////////////////////////////////////////////////////////////////////////

class MruListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MruListWidget(MruList *mru, QWidget *parent = 0);

    void setHeader(const QString& s);

private:
    QPointer<MruList> _mru;
    QVBoxLayout* _layout;
    QMap<QObject*, QAction*> _links;
    QLabel* _header;

protected:
    virtual QString makeLinkText(QAction* action) const;

private slots:
    void clicked();
    void populate();
};

////////////////////////////////////////////////////////////////////////////////

class MruFileListWidget : public MruListWidget
{
public:
    explicit MruFileListWidget(MruList *mru, QWidget *parent = 0) : MruListWidget(mru, parent) {}

protected:
    QString makeLinkText(QAction* action) const override;

private:
    QString filePathColor() const;
};

////////////////////////////////////////////////////////////////////////////////


} // namespace Widgets
} // namespace Ori

#endif // ORI_MRU_MENU_H

