#ifndef ORI_DIALOGS_H
#define ORI_DIALOGS_H

#include <QSize>
#include <QString>
#include <QVector>

#include <functional>

QT_BEGIN_NAMESPACE
class QAbstractButton;
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


namespace Mock {

enum class DialogKind { none, info, warning, error, yes, ok, yesNoCancel };
void setActive(bool on);
void resetLastDialog();
DialogKind getLastDialog();
void setNextResult(int res);
}

/// Shows a dialog for entering a string value.
/// Returns empty string if the dialog was canceled.
QString inputText(const QString& label, const QString& value);

/// Shows a dialog for entering a string value.
/// Assigns the dialog result to `ok` parameter.
/// Returns the initial string if the dialog was canceled.
QString inputText(const QString& label, const QString& value, bool *ok);

QString getSaveFileName(const QString& title, const QString& filter, const QString& defaultExt);

/// Shows a widget in a dialog with OK and Cancel buttons at the bottom.
/// The widget is placed in the dialog alongside with a prompt in a horizontal or vertical layout.
bool showDialogWithPrompt(Qt::Orientation orientation, const QString& prompt, QWidget *widget, const QString& title = QString(), const QString &icon = QString());
bool showDialogWithPromptH(const QString& prompt, QWidget *widget, const QString& title = QString(), const QString &icon = QString());
bool showDialogWithPromptV(const QString& prompt, QWidget *widget, const QString& title = QString(), const QString &icon = QString());

bool show(QDialog* dlg);

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
    Dialog& withHorizontalPrompt(const QString& prompt) { _prompt = prompt; _isPromptVertical = false; return *this; }

    /// Content is placed in dialog alongside with a prompt in vertical layout.
    Dialog& withVerticalPrompt(const QString& prompt) { _prompt = prompt; _isPromptVertical = true; return *this; }

    /// Widget should have slot apply() to process OK button click.
    Dialog& connectOkToContentApply() { _connectOkToContentApply = true; return *this; }

    Dialog& withOkSignal(const char* signal);
    Dialog& withOkSignal(QObject* sender, const char* signal);

    Dialog& withVerification(VerificationFunc verify) { _verify = verify; return *this; }

    Dialog& withInitialSize(const QSize& size) { _initialSize = size; return *this; }

    Dialog& withActiveWidget(QWidget* w) { _activeWidget = w; return *this; }

    Dialog& withOnDlgReady(std::function<void()> handler) { _onDlgReady = handler; return *this; }

    bool exec();

    QSize size() const;

    QAbstractButton* okButton() const { return _okButton; }

private:
    QDialog* _dialog = nullptr;
    QWidget *_content, *_backupContentParent;
    QString _title, _helpTopic, _iconPath, _prompt;
    QBoxLayout* _contentLayout;
    bool _ownContent = false;
    bool _fixedContentSize = true;
    int _contentToButtonsSpacingFactor = 1;
    bool _connectOkToContentApply = false;
    QVector<QPair<QObject*, const char*>> _okSignals;
    bool _isPromptVertical = false;
    VerificationFunc _verify;
    QSize _initialSize;
    QWidget* _activeWidget = nullptr;
    QAbstractButton* _okButton = nullptr;
    std::function<void()> _onDlgReady;

    void makeDialog();
    void acceptDialog();
};

} // namespace Dlg
} // namespace Ori

#endif // ORI_DIALOGS_H
