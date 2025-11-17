#ifndef ORI_WIDGETS_H
#define ORI_WIDGETS_H

#include <QAction>
#include <QKeySequence>
#include <QString>

QT_BEGIN_NAMESPACE
class QBoxLayout;
class QComboBox;
class QGroupBox;
class QLabel;
class QLayout;
class QMenu;
class QObject;
class QPushButton;
class QSpinBox;
class QSplitter;
class QTableView;
class QTextBrowser;
class QToolBar;
class QToolButton;
class QTreeWidget;
class QWidget;
QT_END_NAMESPACE

namespace Ori {
namespace Gui {

void adjustFont(QWidget*);
void setFontSizePt(QWidget *w, int sizePt);
void setFontMonospace(QWidget *w, int sizePt = 0);

QSplitter* splitterH(QWidget*, QWidget*);
QSplitter* splitterV(QWidget*, QWidget*);
QSplitter* splitterH(QWidget*, QWidget*, int size1, int size2);
QSplitter* splitterV(QWidget*, QWidget*, int size1, int size2);
QSplitter* splitterH(QWidget*, int stretch1, QWidget*, int stretch2);
QSplitter* splitterV(QWidget*, int stretch1, QWidget*, int stretch2);

QToolButton* textToolButton(QAction* action);
QToolButton* menuToolButton(QMenu* menu, QAction* action = nullptr);
QToolButton* iconToolButton(const QString& tooltip, const QString& iconPath, QObject* receiver, const char* slot);
QToolButton* iconToolButton(const QString& tooltip, const QString& iconPath, int iconSize, QObject* receiver, const char* slot);

QMenu* menu(std::initializer_list<QObject*> items);
QMenu* menu(QWidget *parent, std::initializer_list<QObject*> items);
QMenu* menu(const QString& title, std::initializer_list<QObject*> items);
QMenu* menu(const QString& title, QWidget *parent, std::initializer_list<QObject*> items);
QMenu* populate(QMenu* menu, std::initializer_list<QObject*> items);
void append(QMenu* menu, QObject* item);

QMenu* makeToggleWidgetsMenu(QMenu* parent, const QString& title, std::initializer_list<QWidget*> widgets);

QToolBar* toolbar(std::initializer_list<QObject*> items);
QToolBar* toolbar(const QString& title, std::initializer_list<QObject*> items);
QToolBar* toolbar(const QString& title, const QString& objectName, std::initializer_list<QObject*> items);
QToolBar* populate(QToolBar* toolbar, std::initializer_list<QObject*> items);
void append(QToolBar* toolbar, QObject* item);

QBoxLayout* layoutH(const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(const std::initializer_list<QObject*>& items);
QBoxLayout* layoutH(int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutH(QWidget* parent, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(QWidget* parent, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutH(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* layoutV(QWidget* parent, int margin, int spacing, const std::initializer_list<QObject*>& items);
QBoxLayout* populate(QBoxLayout* layout, const std::initializer_list<QObject*>& items);
void append(QBoxLayout* layout, QObject* item);
QObject* spacing(int size);
QObject* defaultSpacing(int factor = 1);

QLabel* stretchedLabel(const QString& text);

int layoutSpacing();
int borderWidth();

QGroupBox* group(const QString& title, QLayout* layout);
QGroupBox* groupV(const QString& title, const std::initializer_list<QObject*>& items);
QGroupBox* groupH(const QString& title, const std::initializer_list<QObject*>& items);

QWidget* widget(QBoxLayout* layout);
QWidget* widgetV(const std::initializer_list<QObject*>& items);
QWidget* widgetH(const std::initializer_list<QObject*>& items);

QPushButton* button(const QString& title, QObject* receiver, const char* slot);
QPushButton* iconButton(const QString& tooltip, const QString& iconPath, QObject* receiver, const char* slot, bool flat = false);

void setSelectedId(QComboBox *combo, int id);
int getSelectedId(const QComboBox *combo, int def = -1);

void setActionTooltipFormat(const char* fmt);
namespace V0 {
QAction* action(const QString& title, QObject* receiver, const char* slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence());
QAction* action(const QString& title, const QString& tooltip, QObject* receiver, const char* slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence());
QAction* toggledAction(const QString& title, QObject* receiver, const char* slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence());
QAction* toggledAction(const QString& title, const QString& tooltip, QObject* receiver, const char* slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence());
}

QAction* separatorAction(QObject *parent);

template <typename Func1>
QAction* action(const QString& title, QObject* receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = new QAction(title, receiver);
    if (icon)
        a->setIcon(QIcon(QString(icon)));
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(shortcut)
#else
    a->setShortcut(shortcut);
#endif
    a->connect(a, &QAction::triggered, receiver, slot);
    return a;
}

template <typename Object, typename Func1>
QAction* action(const QString& title, const Object receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = new QAction(title, receiver);
    if (icon)
        a->setIcon(QIcon(QString(icon)));
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(shortcut)
#else
    a->setShortcut(shortcut);
#endif
    a->connect(a, &QAction::triggered, receiver, slot);
    return a;
}

template <typename Func1>
QAction* action(const QString& title, const QString& tooltip, QObject* receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = new QAction(title, receiver);
    if (icon)
        a->setIcon(QIcon(QString(icon)));
    a->setToolTip(tooltip);
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(shortcut)
#else
    a->setShortcut(shortcut);
#endif
    a->connect(a, &QAction::triggered, receiver, slot);
    return a;
}

template <typename Object, typename Func1>
QAction* action(const QString& title, const QString& tooltip, const Object receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = new QAction(title, receiver);
    if (icon)
        a->setIcon(QIcon(QString(icon)));
    a->setToolTip(tooltip);
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(shortcut)
#else
    a->setShortcut(shortcut);
#endif
    a->connect(a, &QAction::triggered, receiver, slot);
    return a;
}

template <typename Func1>
QAction* checkableAction(const QString& title, bool checked, QObject* receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = action(title, receiver, slot, icon, shortcut);
    a->setCheckable(true);
    a->setChecked(checked);
    return a;
}

template <typename Object, typename Func1>
QAction* checkableAction(const QString& title, bool checked, const Object receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = action(title, receiver, slot, icon, shortcut);
    a->setCheckable(true);
    a->setChecked(checked);
    return a;
}

template <typename Func1>
QAction* checkableAction(const QString& title, const QString& tooltip, bool checked, QObject* receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = action(title, tooltip, receiver, slot, icon, shortcut);
    a->setCheckable(true);
    a->setChecked(checked);
    return a;
}

template <typename Object, typename Func1>
QAction* checkableAction(const QString& title, const QString& tooltip, bool checked, const Object receiver, Func1 slot, const char* icon = nullptr, const QKeySequence& shortcut = QKeySequence()) {
    auto a = action(title, tooltip, receiver, slot, icon, shortcut);
    a->setCheckable(true);
    a->setChecked(checked);
    return a;
}

QTreeWidget* twoColumnTree(const QString& title1, const QString& title2);

void stretchColumn(QTableView *table, int col);
void resizeColumnToContent(QTableView *table, int col);

void toggleWidget(QWidget* panel);

QSpinBox* spinBox(int min, int max, int value = 0);

} // namespace Gui
} // namespace Ori

#endif // ORI_WIDGETS_H
