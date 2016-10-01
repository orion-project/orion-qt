#ifndef ORI_DIALOGS_H
#define ORI_DIALOGS_H

#include <QString>

QT_BEGIN_NAMESPACE
class QObject;
class QDialog;
class QWidget;
QT_END_NAMESPACE

namespace Ori {
namespace Dlg {

void info(const QString& msg);
void warning(const QString& msg);
void error(const QString& msg);
bool yes(const QString& msg);
bool ok(const QString& msg);
int yesNoCancel(const QString& msg);
int yesNoCancel(QString& msg);

QString inputText(const QString& label, const QString& value);
QString inputText(const QString& label, const QString& value, bool *ok);

bool show(QDialog* dlg);

/// Shows a widget in a dialog with OK and Cancel buttons at bottom.
/// Receiver should have slot apply() to process OK button click.
bool showDialog(QWidget *widget, QObject *receiver, const QString& title = QString(), const QString& icon = QString());

/// Shows a widget in a dialog with OK and Cancel buttons at bottom.
/// Widget should have slot apply() to process OK button click.
bool showDialog(QWidget *widget, const QString& title = QString(), const QString& icon = QString());

/// Shows a widget in a dialog with OK and Cancel buttons at bottom.
/// Widget is placed in dialog alongside with a prompt in a horizontal or vertical layout.
/// Receiver should have slot apply() to process OK button click.
bool showDialogWithPrompt(Qt::Orientation orientation, const QString& prompt, QWidget *widget, QObject *receiver, const QString& title = QString(), const QString &icon = QString());

/// Shows a widget in a dialog with OK and Cancel buttons at bottom.
/// Widget is placed in dialog alongside with a prompt in a horizontal or vertical layout.
/// Widget should have slot apply() to process OK button click.
bool showDialogWithPrompt(Qt::Orientation orientation, const QString& prompt, QWidget *widget, const QString& title = QString(), const QString &icon = QString());

inline bool showDialogWithPromptH(const QString& prompt, QWidget *widget, QObject *receiver, const QString& title = QString(), const QString &icon = QString())
{
    return showDialogWithPrompt(Qt::Horizontal, prompt, widget, receiver, title, icon);
}

inline bool showDialogWithPromptH(const QString& prompt, QWidget *widget, const QString& title = QString(), const QString &icon = QString())
{
    return showDialogWithPrompt(Qt::Horizontal, prompt, widget, title, icon);
}

inline bool showDialogWithPromptV(const QString& prompt, QWidget *widget, QObject *receiver, const QString& title = QString(), const QString &icon = QString())
{
    return showDialogWithPrompt(Qt::Vertical, prompt, widget, receiver, title, icon);
}

inline bool showDialogWithPromptV(const QString& prompt, QWidget *widget, const QString& title = QString(), const QString &icon = QString())
{
    return showDialogWithPrompt(Qt::Vertical, prompt, widget, title, icon);
}

/// Inserts a widget and dialog buttons panel into a dialog.
/// Receiver should have slot apply() to process OK button click.
void prepareDialog(QDialog *dlg, QWidget *widget, QObject *receiver);

/// Inserts a widget and dialog buttons panel into a dialog.
/// Widget should have slot apply() to process OK button click.
void prepareDialog(QDialog *dlg, QWidget *widget);

} // namespace Dlg
} // namespace Ori

#endif // ORI_DIALOGS_H
