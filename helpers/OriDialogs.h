#ifndef ORI_DIALOGS_H
#define ORI_DIALOGS_H

#include <QSize>
#include <QString>
#include <QVector>

#include <functional>

QT_BEGIN_NAMESPACE
class QObject;
class QDialog;
class QWidget;
class QLayout;
class QBoxLayout;
class QVBoxLayout;
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

QString getSaveFileName(const QString& title, const QString& filter, const QString& defaultExt);

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



/// Shows a content in a dialog with 'OK', 'Cancel' and optional 'Help' buttons at bottom.
class Dialog
{
public:
    typedef std::function<QString()> VerificationFunc;

    Dialog(QWidget* content);
    ~Dialog();

    Dialog& withTitle(const QString& title) { _title = title; return *this; }
    Dialog& withHelpTopic(const QString& topic) { _helpTopic = topic; return *this; }
    Dialog& withIconPath(const QString& iconPath) { _iconPath = iconPath; return *this; }

    /// Content should take all available place when dialog is resized.
    Dialog& withStretchedContent() { _fixedContentSize = false; return *this; }

    /// Append additional space between content and buttons box.
    /// Space is set as amount of default layout spacings.
    Dialog& withContentToButtonsSpacingFactor(int factor) { _contentToButtonsSpacingFactor = factor; return *this; }

    /// Content is placed in dialog alongside with a prompt in horizontal layout.
    Dialog& withHorizontalPrompt(const QString& prompt) { _prompt = prompt, _isPromptVertical = false; return *this; }

    /// Content is placed in dialog alongside with a prompt in vertical layout.
    Dialog& withVerticalPrompt(const QString& prompt) { _prompt = prompt, _isPromptVertical = true; return *this; }

    /// Widget should have slot apply() to process OK button click.
    Dialog& connectOkToContentApply() { _connectOkToContentApply = true; return *this; }

    Dialog& withOkSignal(const char* signal) { _okSignals << signal; return *this; }

    Dialog& withVerification(VerificationFunc verify) { _verify = verify; return *this; }

    Dialog& withInitialSize(const QSize& size) { _initialSize = size; return *this; }

    bool exec();

    QSize size() const;

private:
    QDialog* _dialog = nullptr;
    QWidget *_content, *_backupContentParent;
    QString _title, _helpTopic, _iconPath, _prompt;
    QBoxLayout* _contentLayout;
    bool _ownContent = false;
    bool _fixedContentSize = true;
    int _contentToButtonsSpacingFactor = 1;
    bool _connectOkToContentApply = false;
    QVector<const char*> _okSignals;
    bool _isPromptVertical = false;
    VerificationFunc _verify;
    QSize _initialSize;

    void makeDialog();
    void acceptDialog();
};

} // namespace Dlg
} // namespace Ori

#endif // ORI_DIALOGS_H
