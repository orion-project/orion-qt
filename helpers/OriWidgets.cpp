#include "OriWidgets.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QDebug>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QStyle>
#include <QSpinBox>
#include <QSplitter>
#include <QTableView>
#include <QTextBrowser>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    #define qintptr std::intptr_t
#endif

namespace Ori {
namespace Gui {

//--------------------------------------------------------------------------------------------------

void adjustFont(QWidget *w)
{
#ifdef Q_OS_WIN
    QFont f = w->font();
    if (f.pointSize() < 10)
    {
        f.setPointSize(10);
        w->setFont(f);
    }
#else
    Q_UNUSED(w)
#endif
}

void setFontSizePt(QWidget *w, int sizePt)
{
    QFont f = w->font();
    f.setPointSize(sizePt);
    w->setFont(f);
}

void setFontMonospace(QWidget *w, int sizePt)
{
    QFont f = w->font();

#if defined(Q_OS_WIN)
    f.setFamily("Consolas");
    f.setPointSize(10);
#elif defined(Q_OS_MAC)
    f.setFamily("Monaco");
    f.setPointSize(13);
#else
    f.setFamily("monospace");
    f.setPointSize(11);
#endif
    if (sizePt > 0)
        f.setPointSize(sizePt);
    w->setFont(f);
}

//--------------------------------------------------------------------------------------------------

QSplitter* splitter(Qt::Orientation orientation, QWidget *w1, QWidget *w2, QList<int> sizes)
{
    auto splitter = new QSplitter(orientation);
    splitter->addWidget(w1);
    splitter->addWidget(w2);
    if (!sizes.isEmpty())
        splitter->setSizes(sizes);
    return splitter;
}

QSplitter* splitterH(QWidget *w1, QWidget *w2)
{
    return splitter(Qt::Horizontal, w1, w2, {});
}

QSplitter* splitterV(QWidget *w1, QWidget *w2)
{
    return splitter(Qt::Vertical, w1, w2, {});
}

QSplitter* splitterH(QWidget *w1, QWidget *w2, int size1, int size2)
{
    return splitter(Qt::Horizontal, w1, w2, {size1, size2});
}

QSplitter* splitterV(QWidget *w1, QWidget *w2, int size1, int size2)
{
    return splitter(Qt::Vertical, w1, w2, {size1, size2});
}

//--------------------------------------------------------------------------------------------------

QToolButton* textToolButton(QAction* action)
{
    auto button = new QToolButton;
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    return button;
}

QToolButton* menuToolButton(QMenu* menu, QAction* action)
{
    auto button = new QToolButton;
    button->setDefaultAction(action);
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenu(menu);
    return button;
}

QToolButton* iconToolButton(const QString& tooltip, const QString& iconPath, QObject* receiver, const char* slot)
{
    auto button = new QToolButton;
    button->setToolTip(tooltip);
    button->setIcon(QIcon(iconPath));
    button->connect(button, SIGNAL(clicked()), receiver, slot);
    return button;
}

QToolButton* iconToolButton(const QString& tooltip, const QString& iconPath, int iconSize, QObject* receiver, const char* slot)
{
    auto button = new QToolButton;
    button->setToolTip(tooltip);
    button->setIconSize(QSize(iconSize, iconSize));
    button->setIcon(QIcon(iconPath));
    button->connect(button, SIGNAL(clicked()), receiver, slot);
    return button;
}

//--------------------------------------------------------------------------------------------------

QMenu* menu(std::initializer_list<QObject*> items)
{
    return menu(QString(), nullptr, items);
}

QMenu* menu(QWidget *parent, std::initializer_list<QObject*> items)
{
    return menu(QString(), parent, items);
}

QMenu* menu(const QString& title, std::initializer_list<QObject*> items)
{
    return menu(title, nullptr, items);
}

QMenu* menu(const QString& title, QWidget *parent, std::initializer_list<QObject*> items)
{
    return populate(new QMenu(title, parent), items);
}

QMenu* populate(QMenu* menu, std::initializer_list<QObject*> items)
{
    if (!menu) return nullptr;

    menu->clear();
    for (auto item: items)
        append(menu, item);

    return menu;
}

void append(QMenu* menu, QObject* item)
{
    if (!item)
    {
        menu->addSeparator();
        return;
    }
    auto action = qobject_cast<QAction*>(item);
    if (action)
    {
        menu->addAction(action);
        return;
    }
    auto submenu = qobject_cast<QMenu*>(item);
    if (submenu)
    {
        menu->addMenu(submenu);
        return;
    }
}

QMenu* makeToggleWidgetsMenu(QMenu* parent, const QString& title, std::initializer_list<QWidget*> widgets)
{
    QMenu* menu = parent->addMenu(title);
    QVector<QPair<QAction*, QWidget*> > actions;
    for (QWidget* widget : widgets)
    {
        QAction *action = menu->addAction(widget->windowTitle(), [widget](){ toggleWidget(widget); });
        action->setCheckable(true);
        actions.push_back(QPair<QAction*, QWidget*>(action, widget));
    }
    qApp->connect(menu, &QMenu::aboutToShow, [actions](){
        for (const QPair<QAction*, QWidget*>& pair : actions)
            pair.first->setChecked(pair.second->isVisible());
    });
    return menu;
}

//--------------------------------------------------------------------------------------------------

QToolBar* toolbar(std::initializer_list<QObject*> items)
{
    return populate(new QToolBar, items);
}

QToolBar* toolbar(const QString& title, std::initializer_list<QObject*> items)
{
    return populate(new QToolBar(title), items);
}

QToolBar* toolbar(const QString& title, const QString& objectName, std::initializer_list<QObject*> items)
{
    auto tb = new QToolBar(title);
    tb->setObjectName(objectName);
    return populate(tb, items);
}

QToolBar* populate(QToolBar* toolbar, std::initializer_list<QObject*> items)
{
    if (!toolbar) return nullptr;

    toolbar->clear();
    for (auto item : items)
        append(toolbar, item);

    return toolbar;
}

void append(QToolBar* toolbar, QObject* item)
{
    if (!item)
    {
        toolbar->addSeparator();
        return;
    }
    auto action = qobject_cast<QAction*>(item);
    if (action)
    {
        toolbar->addAction(action);
        return;
    }
    auto group = qobject_cast<QActionGroup*>(item);
    if (group)
    {
        for (auto action : group->actions())
            toolbar->addAction(action);
        return;
    }
    auto widget = qobject_cast<QWidget*>(item);
    if (widget)
    {
        toolbar->addWidget(widget);
        return;
    }
}

//--------------------------------------------------------------------------------------------------

QBoxLayout* layoutH(const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return populate(new QHBoxLayout, items);
}

QBoxLayout* layoutV(const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return populate(new QVBoxLayout, items);
}

QBoxLayout* layoutH(QWidget* parent, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return populate(new QHBoxLayout(parent), items);
}

QBoxLayout* layoutV(QWidget* parent, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return populate(new QVBoxLayout(parent), items);
}

QBoxLayout* initGeometry(QBoxLayout* layout, int margin, int spacing)
{
    if (margin >= 0) layout->setContentsMargins(margin, margin, margin, margin);
    if (spacing >= 0) layout->setSpacing(spacing);
    return layout;
}

QBoxLayout* layoutH(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return initGeometry(layoutH(parent, items), margin, spacing);
}

QBoxLayout* layoutV(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return initGeometry(layoutV(parent, items), margin, spacing);
}

QBoxLayout* layoutH(int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return initGeometry(layoutH(items), margin, spacing);
}

QBoxLayout* layoutV(int margin, int spacing, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return initGeometry(layoutV(items), margin, spacing);
}

QBoxLayout* populate(QBoxLayout* layout, const std::initializer_list<QObject*>& items)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    if (!layout) return nullptr;

    for (auto item: items)
        append(layout, item);

    return layout;
}

void append(QBoxLayout* layout, QObject* item)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    if (!item)
    {
        layout->addStretch();
        return;
    }
    if (qintptr(item) < 100)
    {
        layout->addSpacing(int(qintptr(item)));
        return;
    }
    auto widget = qobject_cast<QWidget*>(item);
    if (widget)
    {
        layout->addWidget(widget);
        return;
    }
    auto sublayout = qobject_cast<QLayout*>(item);
    if (sublayout)
    {
        layout->addLayout(sublayout);
        return;
    }
}

QObject* spacing(int size)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return (QObject*)qintptr(size);
}

QObject* defaultSpacing(int factor)
{
    qDebug() << "This function is obsolete and should be removed. Please use stuff from OriLayout.h instead.";
    return spacing(factor * layoutSpacing());
}

//--------------------------------------------------------------------------------------------------

QLabel* stretchedLabel(const QString& text)
{
    auto label = new QLabel(text);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    return label;
}

//--------------------------------------------------------------------------------------------------

int layoutSpacing()
{
    int spacing = qApp->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
    // MacOS's style "macintosh" has spacing -1, sic!
    // It is not what we want adjusting widgets using layoutSpacing()
    // Fusion style gets 6 and it's most reasonable style in Qt
    if (spacing < 0) spacing = 6;
    return spacing;
}

int borderWidth() { return qApp->style()->pixelMetric(QStyle::PM_DefaultFrameWidth); }

//--------------------------------------------------------------------------------------------------

QGroupBox* group(const QString& title, QLayout *layout)
{
    auto group = new QGroupBox(title);
    group->setLayout(layout);
    return group;
}

QGroupBox* groupV(const QString& title, const std::initializer_list<QObject*>& items)
{
    return group(title, layoutV(items));
}

QGroupBox* groupH(const QString& title, const std::initializer_list<QObject*>& items)
{
    return group(title, layoutH(items));
}

//--------------------------------------------------------------------------------------------------

QWidget* widget(QBoxLayout* layout)
{
    auto widget = new QWidget;
    widget->setLayout(layout);
    return widget;
}

QWidget* widgetV(const std::initializer_list<QObject*>& items)
{
    return widget(layoutV(items));
}

QWidget* widgetH(const std::initializer_list<QObject*>& items)
{
    return widget(layoutH(items));
}

//--------------------------------------------------------------------------------------------------

QPushButton* button(const QString& title, QObject *receiver, const char* slot)
{
    auto button = new QPushButton(title);
    button->connect(button, SIGNAL(clicked(bool)), receiver, slot);
    return button;
}

QPushButton* iconButton(const QString& tooltip, const QString& iconPath, QObject* receiver, const char* slot, bool flat)
{
    auto button = new QPushButton;
    button->setIcon(QIcon(iconPath));
    button->setToolTip(tooltip);
    button->connect(button, SIGNAL(clicked(bool)), receiver, slot);
    button->setFlat(flat);
    return button;
}

//--------------------------------------------------------------------------------------------------

void setSelectedId(QComboBox *combo, int id)
{
    for (int i = 0; i < combo->count(); i++)
        if (combo->itemData(i).toInt() == id)
        {
            combo->setCurrentIndex(i);
            return;
        }
    combo->setCurrentIndex(-1);
}

int getSelectedId(const QComboBox *combo, int def)
{
    QVariant data = combo->itemData(combo->currentIndex());
    if (!data.isValid()) return def;
    bool ok;
    int id = data.toInt(&ok);
    return ok? id: def;
}

//--------------------------------------------------------------------------------------------------

// Guesses a descriptive text suited for the menu entry.
// This is equivalent to QAction's internal qt_strippedText().
static QString strippedActionTitle(QString s) {
    s.remove(QString::fromLatin1("..."));
    for (int i = 0; i < s.size(); ++i)
        if (s.at(i) == QLatin1Char('&'))
            s.remove(i, 1);
    return s.trimmed();
}

static const char* __actionTooltipFormat("<p style='white-space:pre'>%1&nbsp;&nbsp;(<code>%2</code>)</p>");

void setActionTooltipFormat(const char *fmt)
{
    __actionTooltipFormat = fmt;
}

// Adds shortcut information to the action's tooltip.
// Here is more complete solution supporting custom tooltips
// https://stackoverflow.com/questions/42607554/show-shortcut-in-tooltip-of-qtoolbar
static void setActionTooltip(QAction* action, const QString& tooltip, const QKeySequence& shortcut)
{
    QString t = tooltip;

    if (t.isEmpty())
        t = action->text();

    t = strippedActionTitle(t);

    if (!shortcut.isEmpty() && strlen(__actionTooltipFormat) > 0)
        t = QString::fromLatin1(__actionTooltipFormat).arg(t, shortcut.toString(QKeySequence::NativeText));

    action->setToolTip(t);
}

namespace V0 {

QAction* action(const QString& title, const QString& tooltip, QObject* receiver, const char* slot, const char* icon, const QKeySequence& shortcut)
{
    auto action = new QAction(title, receiver);
    setActionTooltip(action, tooltip, shortcut);
    if (!shortcut.isEmpty()) action->setShortcut(shortcut);
    if (icon) action->setIcon(QIcon(icon));
    qApp->connect(action, SIGNAL(triggered()), receiver, slot);
    return action;
}

QAction* action(const QString& title, QObject* receiver, const char* slot, const char* icon, const QKeySequence& shortcut)
{
    return action(title, title, receiver, slot, icon, shortcut);
}

QAction* toggledAction(const QString& title, QObject* receiver, const char* slot, const char* icon, const QKeySequence& shortcut)
{
    return toggledAction(title, title, receiver, slot, icon, shortcut);
}

QAction* toggledAction(const QString& title, const QString& tooltip, QObject* receiver, const char* slot, const char* icon, const QKeySequence& shortcut)
{
    auto action = new QAction(title, receiver);
    action->setCheckable(true);
    setActionTooltip(action, tooltip, shortcut);
    if (!shortcut.isEmpty()) action->setShortcut(shortcut);
    if (icon) action->setIcon(QIcon(icon));
    if (slot) qApp->connect(action, SIGNAL(toggled(bool)), receiver, slot);
    return action;
}

} // namespace V0

//--------------------------------------------------------------------------------------------------

QTreeWidget* twoColumnTree(const QString& title1, const QString& title2)
{
    auto tree = new QTreeWidget;
    tree->setColumnCount(2);
    tree->setAlternatingRowColors(true);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
#else
    tree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    tree->header()->setResizeMode(1, QHeaderView::Stretch);
#endif

    auto header = new QTreeWidgetItem();
    header->setText(0, title1);
    header->setText(1, title2);
    tree->setHeaderItem(header);

    return tree;
}

//--------------------------------------------------------------------------------------------------

void stretchColumn(QTableView *table, int col)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    #else
        table->horizontalHeader()->setResizeMode(col, QHeaderView::Stretch);
    #endif
}

void resizeColumnToContent(QTableView *table, int col)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    #else
        table->horizontalHeader()->setResizeMode(col, QHeaderView::ResizeToContents);
    #endif
}

//--------------------------------------------------------------------------------------------------

void toggleWidget(QWidget* panel)
{
    if (panel->isVisible()) panel->hide(); else panel->show();
}

//--------------------------------------------------------------------------------------------------

QSpinBox* spinBox(int min, int max, int value)
{
    auto sb = new QSpinBox;
    sb->setMinimum(min);
    sb->setMaximum(max);
    if (value != 0)
        sb->setValue(value);
    return sb;
}

} // namespace Gui
} // namespace Ori


