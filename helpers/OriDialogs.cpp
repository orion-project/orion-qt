#include "OriDialogs.h"

#include <QAction>
#include <QApplication>
#include <QAbstractButton>
#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGlobalStatic>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>

namespace Ori {
namespace Dlg {

namespace Mock {

static bool isActive = false;
static DialogKind lastDialog = DialogKind::none;
static int nextResult = QMessageBox::StandardButton::NoButton;

void setActive(bool on) { isActive = on; }
void resetLastDialog() { lastDialog = DialogKind::none; }
DialogKind getLastDialog() { return lastDialog; }
void setNextResult(int res) { nextResult = res; }

} // namespace Mock

void info(const QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::info;
        return;
    }

    QMessageBox::information(qApp->activeWindow(), qApp->applicationName(), msg, QMessageBox::Ok, QMessageBox::Ok);
}

void warning(const QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::warning;
        return;
    }

    QMessageBox::warning(qApp->activeWindow(), qApp->applicationName(), msg, QMessageBox::Ok, QMessageBox::Ok);
}

void error(const QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::error;
        return;
    }

    QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(), msg, QMessageBox::Ok, QMessageBox::Ok);
}

bool yes(const QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::yes;
        return Mock::nextResult == QMessageBox::Yes;
    }

    return (QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes);
}

bool ok(const QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::ok;
        return Mock::nextResult == QMessageBox::Ok;
    }

    return (QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok);
}

int yesNoCancel(const QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::yesNoCancel;
        return Mock::nextResult;
    }

    return QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
}

int yesNoCancel(QString& msg)
{
    if (Mock::isActive)
    {
        Mock::lastDialog = Mock::DialogKind::yesNoCancel;
        return Mock::nextResult;
    }

    return QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
}

std::optional<bool> yesNo(const QString& msg)
{
    int res = QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
    if (res == QMessageBox::Yes)
        return true;
    if (res == QMessageBox::No)
        return false;
    return {};
}

//------------------------------------------------------------------------------
//                         Ori::Dlg::InputTextEditor
//------------------------------------------------------------------------------

InputTextEditor::InputTextEditor(QWidget* parent) : QLineEdit(parent)
{
}

QSize InputTextEditor::sizeHint() const
{
    auto s = QLineEdit::sizeHint();
    s.setWidth(s.width() * _widthFactor);
    return s;
}

//------------------------------------------------------------------------------
//                            Ori::Dlg::inputText
//------------------------------------------------------------------------------

QString inputText(const QString& label, const QString& value)
{
    bool ok;
    QString text = inputText(label, value, &ok);
    return ok? text: QString();
}

QString inputText(const QString& label, const QString& value, bool *ok)
{
    auto newValue = value;

    auto editor = new InputTextEditor();
    editor->setText(value);
    auto s = editor->size();
    editor->resize(s.width() * 2, s.height());

    QWidget content;
    auto layout = new QVBoxLayout(&content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(label));
    layout->addWidget(editor);

    *ok = Dialog(&content, false)
            .withActiveWidget(editor)
            .withContentToButtonsSpacingFactor(2)
            .exec();

    if (*ok)
        newValue = editor->text();

    return newValue;
}

bool inputText(InputTextOptions &opts)
{
    auto editor = new InputTextEditor();
    editor->setText(opts.value);
    auto s = editor->size();
    editor->resize(s.width() * 2, s.height());
    if (opts.maxLength > 0)
        editor->setMaxLength(opts.maxLength);

    QWidget content;
    auto layout = new QVBoxLayout(&content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(opts.label));
    layout->addWidget(editor);

    if (!Dialog(&content, false)
            .withActiveWidget(editor)
            .withOnHelp(opts.onHelp)
            .withContentToButtonsSpacingFactor(2)
            .windowModal()
            .exec())
        return false;

    auto newValue = editor->text();
    if (opts.trimValue)
        newValue = newValue.trimmed();
    if (opts.rejectEmpty && newValue.isEmpty())
        return false;
    if (opts.rejectSame && newValue == opts.value)
        return false;
    opts.value = newValue;
    return true;
}

//------------------------------------------------------------------------------

QString getSaveFileName(const QString& title, const QString& filter, const QString& defaultExt)
{
    QString fileName = QFileDialog::getSaveFileName(qApp->activeWindow(), title, QString(), filter);
    if (fileName.isEmpty()) return QString();

    if (QFileInfo(fileName).suffix().isEmpty())
    {
        if (fileName.endsWith('.'))
            return fileName + defaultExt;
        return fileName + '.' + defaultExt;
    }

    return fileName;
}

//------------------------------------------------------------------------------
//                        Ori::Dlg::showDialogWithPrompt
//------------------------------------------------------------------------------

bool showDialogWithPrompt(Qt::Orientation orientation, const QString& prompt, QWidget *widget, const QString& title, const QString &icon)
{
    auto oldParent = qobject_cast<QWidget*>(widget->parent());

    QWidget content;
    QBoxLayout *layout;
    if (orientation == Qt::Vertical)
        layout = new QVBoxLayout(&content);
    else
        layout = new QHBoxLayout(&content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(prompt));
    layout->addWidget(widget);
    bool ok = Dialog(&content, false)
            .withTitle(title)
            .withIconPath(icon)
            .withContentToButtonsSpacingFactor(2)
            .exec();

    // Restore parent to prevent the layout from deletion the widget
    widget->setParent(oldParent);

    return ok;
}

bool showDialogWithPromptH(const QString& prompt, QWidget *widget, const QString& title, const QString &icon)
{
    return showDialogWithPrompt(Qt::Horizontal, prompt, widget, title, icon);
}

bool showDialogWithPromptV(const QString& prompt, QWidget *widget, const QString& title, const QString &icon)
{
    return showDialogWithPrompt(Qt::Vertical, prompt, widget, title, icon);
}

//------------------------------------------------------------------------------

bool show(QDialog* dlg)
{
    return dlg->exec() == QDialog::Accepted;
}

void setDlgTitle(QWidget *dlg, const QString& title)
{
    dlg->setWindowTitle(title.isEmpty()? qApp->applicationName(): title);
}

/**
    Assign dialog window icon.

    On MacOS the icon of active dialog overrides application icon on the dock.
    It is not what we want. As no icon is displayed in window titlebar so we have nothing to do.

    Ubuntu Unity does not display icons in window titlebar,
    but there are another desktops that can show icons (xfce, KDE).
*/
void setDlgIcon(QWidget *dlg, const QString &path)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(dlg)
    Q_UNUSED(path)
#else
    if (path.isEmpty()) return;
    QIcon icon(path);
    if (icon.isNull()) return;
    dlg->setWindowIcon(icon);
#endif
}

//------------------------------------------------------------------------------
//                             Ori::Dlg::Dialog
//------------------------------------------------------------------------------

namespace {
using SavedSizesMap = QMap<QString, QRect>;
Q_GLOBAL_STATIC(SavedSizesMap, __savedSizes)
}


class OriDialogHelpLabel : public QLabel
{
public:
    OriDialogHelpLabel(QWidget *parent) : QLabel(parent) {}

    Dialog::HandlerFunc onHelpRequested;

protected:
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        QLabel::mouseReleaseEvent(e);
        if (e->button() == Qt::LeftButton)
            onHelpRequested();
    }
};


class OriDialog : public QDialog
{
public:
    OriDialog() : QDialog(qApp->activeWindow()) {}

    OriDialogHelpLabel *helpLabel = nullptr;
    Dialog::HandlerFunc onShow;

protected:
    void resizeEvent(QResizeEvent *e) override
    {
        QDialog::resizeEvent(e);
        if (helpLabel)
            helpLabel->move({width() - helpLabel->width() - 3, 3});
    }

    void showEvent(QShowEvent *e) override
    {
        QDialog::showEvent(e);
        if (onShow && !_onShowCalled) {
            _onShowCalled = true;
            onShow();
        }
    }

private:
    bool _onShowCalled = false;
};

Dialog::Dialog(QWidget* content, bool ownContent): _content(content), _ownContent(ownContent)
{
    _backupContentParent = _content->parentWidget();
}

Dialog::~Dialog()
{
    if (_sizeSaver)
        _sizeSaver(_dialog->size());
    if (_dialog) delete _dialog;
}

bool Dialog::exec()
{
    if (!_dialog) makeDialog();
    if (_onDlgShown)
        _dialog->onShow = _onDlgShown;
    if (_activeWidget)
        _activeWidget->setFocus();
    bool res = _dialog->exec() == QDialog::Accepted;
    if (!_ownContent)
    {
        // Restoring ownership prevents widget deletion together with layout
        _contentLayout->removeWidget(_content);
        _content->setParent(_backupContentParent);
    }
    if (!_persistenceId.isEmpty())
        (*__savedSizes)[_persistenceId] = _dialog->geometry();
    return res;
}

void Dialog::makeDialog()
{
    // Dialog window
    _dialog = new OriDialog();
    if (_windowModal)
        _dialog->setWindowModality(Qt::WindowModal);

    auto flags = _dialog->windowFlags();
    flags.setFlag(Qt::WindowContextHelpButtonHint, false);
    _dialog->setWindowFlags(flags);

    setDlgTitle(_dialog, _title);
    setDlgIcon(_dialog, _iconPath);
    if (!_initialSize.isEmpty())
        _dialog->resize(_initialSize);
    if (!_persistenceId.isEmpty() && __savedSizes->contains(_persistenceId))
        _dialog->setGeometry((*__savedSizes)[_persistenceId]);
    QVBoxLayout* dialogLayout = new QVBoxLayout(_dialog);

    // Dialog content
    if (!_prompt.isEmpty())
    {
        if (_isPromptVertical)
            _contentLayout = new QVBoxLayout;
        else
            _contentLayout = new QHBoxLayout;
        _contentLayout->setContentsMargins(0, 0, 0, 0);
        _contentLayout->addWidget(new QLabel(_prompt));
        dialogLayout->addLayout(_contentLayout);
    }
    else
    {
        _contentLayout = dialogLayout;
        if (_skipContentMargins)
            _contentLayout->setContentsMargins(0, 0, 0, 0);
    }
    _contentLayout->addWidget(_content);

    auto style = qApp->style();

    // Content-to-buttons space
    if (_fixedContentSize)
        dialogLayout->addStretch();
    if (_contentToButtonsSpacingFactor > 1)
    {
        int defaultSpacing = style->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
        dialogLayout->addSpacing(defaultSpacing * (_contentToButtonsSpacingFactor - 1));
    }

    // Dialog buttons
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
        ((_onHelpRequested && _helpIcon.isNull()) ? QDialogButtonBox::Help : QDialogButtonBox::NoButton) |
        (_applyHandler ? QDialogButtonBox::Apply : QDialogButtonBox::NoButton)
    );
    qApp->connect(buttonBox, &QDialogButtonBox::accepted, _dialog, [this]{ acceptDialog(); });
    qApp->connect(buttonBox, &QDialogButtonBox::rejected, _dialog, &QDialog::reject);
    if (_connectOkToContentApply)
        qApp->connect(_dialog, SIGNAL(accepted()), _content, SLOT(apply()));
    foreach (const auto& signal, _acceptSignals)
        qApp->connect(signal.first ? signal.first : _content, signal.second, _dialog, SLOT(accept()));
    if (_applyHandler)
        qApp->connect(buttonBox, &QDialogButtonBox::clicked, _dialog, [buttonBox, this](QAbstractButton *button){
            if ((void*)button == (void*)buttonBox->button(QDialogButtonBox::Apply))
                _applyHandler();
        });
    if (_onHelpRequested)
    {
        if (_helpIcon.isEmpty())
            qApp->connect(buttonBox, &QDialogButtonBox::helpRequested, _onHelpRequested);
        else
        {
            auto helpAction = new QAction(_dialog);
            helpAction->setShortcut(QKeySequence::HelpContents);
            helpAction->connect(helpAction, &QAction::triggered, _onHelpRequested);
            _dialog->addAction(helpAction);

            _dialog->helpLabel = new OriDialogHelpLabel(_dialog);
            _dialog->helpLabel->setFixedSize(16, 16);
            _dialog->helpLabel->setCursor(Qt::PointingHandCursor);
            _dialog->helpLabel->setToolTip(qApp->tr("Show Help"));
            _dialog->helpLabel->setPixmap(QIcon(_helpIcon).pixmap(16, 16));
            _dialog->helpLabel->onHelpRequested = _onHelpRequested;
        }
    }

    // By default dialogLayout provides margins
    // When skipping content margins we still want to have margins around buttons
    if (_skipContentMargins)
        buttonBox->setContentsMargins(
            style->pixelMetric(QStyle::PM_LayoutLeftMargin),
            0,
            style->pixelMetric(QStyle::PM_LayoutRightMargin),
            style->pixelMetric(QStyle::PM_LayoutBottomMargin));

    if (_buttonsSeparator)
    {
        auto sep = new QFrame;
        sep->setFrameShape(QFrame::HLine);
        sep->setForegroundRole(QPalette::Mid);
        dialogLayout->addWidget(sep);
    }
    dialogLayout->addWidget(buttonBox);

    foreach (auto button, buttonBox->buttons())
        if (buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
        {
            _okButton = button;
            break;
        }
    if (_okButton && !_okTitle.isEmpty())
        _okButton->setText(_okTitle);

    if (_onDlgReady) _onDlgReady();
}

void Dialog::acceptDialog()
{
    if (_verify)
    {
        QString res = _verify();
        if (!res.isEmpty())
        {
            warning(res);
            return;
        }
    }
    _dialog->accept();
}

QSize Dialog::size() const
{
    return _dialog ? _dialog->size() : QSize();
}

Dialog& Dialog::withAcceptSignal(const char* signal)
{
    _acceptSignals << QPair<QObject*, const char*>(nullptr, signal);
    return *this;
}

Dialog& Dialog::withAcceptSignal(QObject* sender, const char* signal)
{
    _acceptSignals << QPair<QObject*, const char*>(sender, signal);
    return *this;
}

} // namespace Dlg
} // namespace Ori
