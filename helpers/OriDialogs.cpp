#include "OriDialogs.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QStyle>
#include <QMessageBox>

namespace Ori {
namespace Dlg {

void info(const QString& msg)
{
    QMessageBox::information(qApp->activeWindow(), qApp->applicationName(), msg, QMessageBox::Ok, QMessageBox::Ok);
}

void warning(const QString& msg)
{
    QMessageBox::warning(qApp->activeWindow(), qApp->applicationName(), msg, QMessageBox::Ok, QMessageBox::Ok);
}

void error(const QString& msg)
{
    QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(), msg, QMessageBox::Ok, QMessageBox::Ok);
}

bool yes(const QString& msg)
{
    return (QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes);
}

bool ok(const QString& msg)
{
    return (QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok);
}

int yesNoCancel(const QString& msg)
{
    return QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
}

int yesNoCancel(QString& msg)
{
    return QMessageBox::question(qApp->activeWindow(), qApp->applicationName(), msg,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
}

//------------------------------------------------------------------------------
//                            Ori::Dlg::inputText
//------------------------------------------------------------------------------

namespace {
class InputTextEditor : public QLineEdit
{
public:
    InputTextEditor() {}

    QSize sizeHint() const override
    {
        auto s = QLineEdit::sizeHint();
        s.setWidth(s.width() * 2);
        return s;
    }
};
}

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
    layout->setMargin(0);
    layout->addWidget(new QLabel(label));
    layout->addWidget(editor);

    *ok = Dialog(&content)
            .withContentToButtonsSpacingFactor(2)
            .exec();

    if (*ok)
        newValue = editor->text();

    return newValue;
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
    layout->setMargin(0);
    layout->addWidget(new QLabel(prompt));
    layout->addWidget(widget);
    bool ok = Dialog(&content)
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

Dialog::Dialog(QWidget* content): _content(content)
{
    _backupContentParent = _content->parentWidget();
}

Dialog::~Dialog()
{
    if (_dialog) delete _dialog;
}

bool Dialog::exec()
{
    if (!_dialog) makeDialog();
    if (_activeWidget) _activeWidget->setFocus();
    bool res = _dialog->exec() == QDialog::Accepted;
    if (!_ownContent)
    {
        // Restoring ownership prevents widget deletion together with layout
        _contentLayout->removeWidget(_content);
        _content->setParent(_backupContentParent);
    }
    return res;
}

void Dialog::makeDialog()
{
    // Dialog window
    _dialog = new QDialog(qApp->activeWindow());

    auto flags = _dialog->windowFlags();
    flags.setFlag(Qt::WindowContextHelpButtonHint, false);
    _dialog->setWindowFlags(flags);

    setDlgTitle(_dialog, _title);
    setDlgIcon(_dialog, _iconPath);
    if (!_initialSize.isEmpty())
        _dialog->resize(_initialSize);
    QVBoxLayout* dialogLayout = new QVBoxLayout(_dialog);

    // Dialog content
    if (!_prompt.isEmpty())
    {
        if (_isPromptVertical)
            _contentLayout = new QVBoxLayout;
        else
            _contentLayout = new QHBoxLayout;
        _contentLayout->setMargin(0);
        _contentLayout->addWidget(new QLabel(_prompt));
        dialogLayout->addLayout(_contentLayout);
    }
    else
    {
        _contentLayout = dialogLayout;
    }
    _contentLayout->addWidget(_content);

    // Content-to-buttons space
    if (_fixedContentSize)
        dialogLayout->addStretch();
    if (_contentToButtonsSpacingFactor > 1)
    {
        int defaultSpacing = qApp->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
        dialogLayout->addSpacing(defaultSpacing * (_contentToButtonsSpacingFactor - 1));
    }

    // Dialog buttons
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
        (_helpTopic.isEmpty() ? QDialogButtonBox::NoButton : QDialogButtonBox::Help));
    qApp->connect(buttonBox, &QDialogButtonBox::accepted, [this](){ this->acceptDialog(); });
    qApp->connect(buttonBox, &QDialogButtonBox::rejected, _dialog, &QDialog::reject);
    if (_connectOkToContentApply)
        qApp->connect(_dialog, SIGNAL(accepted()), _content, SLOT(apply()));
    for (auto signal: _okSignals)
        qApp->connect(signal.first ? signal.first : _content, signal.second, _dialog, SLOT(accept()));
    if (!_helpTopic.isEmpty()) // TODO process help
        qApp->connect(buttonBox, &QDialogButtonBox::helpRequested, [this](){
            info(QString("TODO help by topic '%1'").arg(this->_helpTopic));
        });
    dialogLayout->addWidget(buttonBox);
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

Dialog& Dialog::withOkSignal(const char* signal)
{
    _okSignals << QPair<QObject*, const char*>(nullptr, signal);
    return *this;
}

Dialog& Dialog::withOkSignal(QObject* sender, const char* signal)
{
    _okSignals << QPair<QObject*, const char*>(sender, signal);
    return *this;
}

} // namespace Dlg
} // namespace Ori
