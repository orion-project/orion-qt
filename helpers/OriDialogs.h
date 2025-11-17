#ifndef ORI_DIALOGS_H
#define ORI_DIALOGS_H

#include <QLineEdit>
//#include <QSize>
//#include <QVector>

#include <functional>
#include <optional>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QBoxLayout;
class QLabel;
class QLayout;
class QObject;
class QVBoxLayout;
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
std::optional<bool> yesNo(const QString& msg);


namespace Mock {

enum class DialogKind { none, info, warning, error, yes, ok, yesNoCancel };
void setActive(bool on);
void resetLastDialog();
DialogKind getLastDialog();
void setNextResult(int res);
}

/// The thin wrapper around QLineEdit. QLineEdit itself is too narrow by default.
/// The wrapper allows setting a new default width as a factor of the 'default default' one.
class InputTextEditor : public QLineEdit
{
public:
    explicit InputTextEditor(QWidget* parent = nullptr);

    void setWidthFactor(int factor) { _widthFactor = factor; }
    QSize sizeHint() const override;

private:
    int _widthFactor = 2;
};

/// Shows a dialog for entering a string value.
/// Returns empty string if the dialog was canceled.
QString inputText(const QString& label, const QString& value);

/// Shows a dialog for entering a string value.
/// Assigns the dialog result to `ok` parameter.
/// Returns the initial string if the dialog was canceled.
QString inputText(const QString& label, const QString& value, bool *ok);

struct InputTextOptions
{
    QString label;
    QString value; // in|out
    std::function<void()> onHelp;
    int maxLength = 0;
    bool trimValue = true;
    bool rejectSame = true;
    bool rejectEmpty = false;
};
bool inputText(InputTextOptions &opts);

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
    typedef std::function<void()> HandlerFunc;

    Dialog(QWidget* content, bool ownContent);
    Dialog(QSharedPointer<QWidget> content) : Dialog(content.get(), false) {}
    ~Dialog();

    Dialog& withTitle(const QString& title) { _title = title; return *this; }
    Dialog& withIconPath(const QString& iconPath) { _iconPath = iconPath; return *this; }

    /// Content should take all available place when dialog is resized.
    Dialog& withStretchedContent() { _fixedContentSize = false; return *this; }

    /// Append additional space between content and buttons box.
    /// Space is set as amount of default layout spacings.
    Dialog& withContentToButtonsSpacingFactor(int factor) { _contentToButtonsSpacingFactor = factor; return *this; }

    /// Don't create default layout margins around provided content
    Dialog& withSkipContentMargins() { _skipContentMargins = true; return *this; }

    /// Content is placed in dialog alongside with a prompt in horizontal layout.
    Dialog& withHorizontalPrompt(const QString& prompt) { _prompt = prompt; _isPromptVertical = false; return *this; }

    /// Content is placed in dialog alongside with a prompt in vertical layout.
    Dialog& withVerticalPrompt(const QString& prompt) { _prompt = prompt; _isPromptVertical = true; return *this; }

    /// Widget should have slot apply() to process OK button click.
    Dialog& connectOkToContentApply() { _connectOkToContentApply = true; return *this; }

    /// The same as withAcceptSignal, deprecated.
    Dialog& withOkSignal(const char* signal) { return withAcceptSignal(signal); }
    Dialog& withOkSignal(QObject* sender, const char* signal) { return withAcceptSignal(sender, signal); }

    /// Override the OK button title.
    Dialog& withAcceptTitle(const QString& title) { _okTitle = title; return *this; }

    /// A signal that must trigger the dialog's accept method.
    /// This way a content can accept the dialog.
    Dialog& withAcceptSignal(const char* signal);
    Dialog& withAcceptSignal(QObject* sender, const char* signal);

    Dialog& withVerification(VerificationFunc verify) { _verify = verify; return *this; }

    Dialog& withInitialSize(const QSize& size) { _initialSize = size; return *this; }
    Dialog& withSizeSaver(std::function<void(QSize)> saver) { _sizeSaver = saver; return *this; }

    Dialog& withActiveWidget(QWidget* w) { _activeWidget = w; return *this; }

    /// A handler that is called when the dialog is ready but yet shown.
    Dialog& withOnDlgReady(HandlerFunc handler) { _onDlgReady = handler; return *this; }

    /// A handler that is called from the dialog showEvent
    Dialog& withOnDlgShown(HandlerFunc handler) { _onDlgShown = handler; return *this; }

    /// A handler that is called when the Help button pressed.
    /// The Help button is not shown if this hander is not provided.
    Dialog& withOnHelp(HandlerFunc handler) { _onHelpRequested = handler; return *this; }
    Dialog& withHelpIcon(const QString& icon) { _helpIcon = icon; return *this; }

    /// A handler that is called when the Apply button pressed.
    /// The Apply button is not shown if this hander is not provided.
    Dialog& withOnApply(HandlerFunc handler) { _applyHandler = handler; return *this; }

    Dialog& withFocusSetter(HandlerFunc handler) { _applyHandler = handler; return *this; }

    /// If the id is provided then the dialog size will be stored in memory
    /// after dialog was closed and then restored on the next run.
    Dialog& withPersistenceId(const QString& id) { _persistenceId = id; return *this; }

    Dialog& withButtonsSeparator() { _buttonsSeparator = true; return *this; }

    Dialog& windowModal() { _windowModal = true; return *this; }

    bool exec();

    QSize size() const;

    QAbstractButton* okButton() const { return _okButton; }

private:
    class OriDialog* _dialog = nullptr;
    QWidget *_content, *_backupContentParent;
    QString _title, _iconPath, _prompt;
    QBoxLayout* _contentLayout;
    bool _ownContent = false;
    bool _fixedContentSize = true;
    int _contentToButtonsSpacingFactor = 1;
    bool _connectOkToContentApply = false;
    QVector<QPair<QObject*, const char*>> _acceptSignals;
    bool _isPromptVertical = false;
    VerificationFunc _verify;
    QSize _initialSize;
    QWidget* _activeWidget = nullptr;
    QAbstractButton* _okButton = nullptr;
    HandlerFunc _onDlgReady;
    HandlerFunc _onDlgShown;
    HandlerFunc _onHelpRequested;
    HandlerFunc _applyHandler;
    std::function<void(QSize)> _sizeSaver;
    QString _persistenceId;
    bool _skipContentMargins = false;
    bool _windowModal = false;
    QString _helpIcon;
    bool _buttonsSeparator = false;
    QString _okTitle;

    void makeDialog();
    void acceptDialog();
};

} // namespace Dlg
} // namespace Ori

#endif // ORI_DIALOGS_H
