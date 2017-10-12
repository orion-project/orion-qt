#include "OriDialogs.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
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

QString inputText(const QString& label, const QString& value)
{
    bool ok;
    QString text = QInputDialog::getText(qApp->activeWindow(), qApp->applicationName(),
                                         label, QLineEdit::Normal, value, &ok);
    return ok? text: QString();
}

QString inputText(const QString& label, const QString& value, bool *ok)
{
    return QInputDialog::getText(qApp->activeWindow(), qApp->applicationName(),
                                 label, QLineEdit::Normal, value, ok);
}

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

bool show(QDialog* dlg)
{
    return dlg->exec() == QDialog::Accepted;
}

void setDlgTitle(QWidget *dlg, const QString& title)
{
    dlg->setWindowTitle(title.isEmpty()? qApp->applicationName(): title);
}

void setDlgIcon(QWidget *dlg, const QString &path)
{
    if (path.isEmpty()) return;
    QIcon icon(path);
    if (icon.isNull()) return;
    dlg->setWindowIcon(icon);
}

bool showDialog(QWidget *widget, const QString& title, const QString &icon)
{
    return showDialog(widget, widget, title, icon);
}

bool showDialog(QWidget *widget, QObject *receiver, const QString& title, const QString& icon)
{
    qDebug() << "This function is obsolete and should be removed. Please use features of Ori::Dlg::Dialog instead.";

    if (!widget) return false;

    auto oldParent = widget->parentWidget();

    QDialog dlg(qApp->activeWindow());
    setDlgTitle(&dlg, title);
    setDlgIcon(&dlg, icon);
    prepareDialog(&dlg, widget, receiver);
    bool ok = show(&dlg);

    // Restoring ownership prevent widget deletion together with layout
    dlg.layout()->removeWidget(widget);
    widget->setParent(oldParent);

    return ok;
}

QBoxLayout* makePromptLayout(Qt::Orientation orientation)
{
    switch (orientation)
    {
    case Qt::Horizontal: return new QHBoxLayout;
    case Qt::Vertical: return new QVBoxLayout;
    default:
        qCritical() << "Unsupported orientation" << orientation;
        return nullptr;
    }
}

bool showDialogWithPrompt(Qt::Orientation orientation, const QString& prompt, QWidget *widget, const QString& title, const QString &icon)
{
    return showDialogWithPrompt(orientation, prompt, widget, widget, title, icon);
}

bool showDialogWithPrompt(Qt::Orientation orientation, const QString& prompt, QWidget *widget, QObject * receiver, const QString& title, const QString& icon)
{
    qDebug() << "This function is obsolete and should be removed. Please use features of Ori::Dlg::Dialog instead.";

    if (!widget) return false;

    auto oldParent = widget->parentWidget();

    auto layout = makePromptLayout(orientation);
    if (!layout) return false;
    layout->setMargin(0);
    layout->addWidget(new QLabel(prompt));
    layout->addWidget(widget);

    QWidget w; w.setLayout(layout);
    bool ok = showDialog(&w, receiver, title, icon);

    // Restoring ownership prevent widget deletion together with layout
    widget->setParent(oldParent);

    return ok;
}

void prepareDialog(QDialog *dlg, QWidget *widget)
{
    prepareDialog(dlg, widget, widget);
}

void prepareDialog(QDialog *dlg, QWidget *widget, QObject *receiver)
{
    qDebug() << "This function is obsolete and should be removed. Please use features of Ori::Dlg::Dialog instead.";

    if (!dlg || !widget) return;

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    qApp->connect(buttons, SIGNAL(accepted()), dlg, SLOT(accept()));
    qApp->connect(buttons, SIGNAL(rejected()), dlg, SLOT(reject()));
    if (receiver)
        qApp->connect(buttons, SIGNAL(accepted()), receiver, SLOT(apply()));

    // We will have margins already in dialogLayout, these are excessed
    if (widget->layout()) widget->layout()->setMargin(0);

    auto dialogLayout = new QVBoxLayout(dlg);
    dialogLayout->addWidget(widget);
    dialogLayout->addWidget(buttons);
}

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
    bool res = _dialog->exec() == QDialog::Accepted;
    if (!_ownContent)
    {
        // Restoring ownership prevent widget deletion together with layout
        _contentLayout->removeWidget(_content);
        _content->setParent(_backupContentParent);
    }
    return res;
}

void Dialog::makeDialog()
{
    _dialog = new QDialog(qApp->activeWindow());

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
        (_helpTopic.isEmpty() ? QDialogButtonBox::NoButton : QDialogButtonBox::Help));
    qApp->connect(buttonBox, &QDialogButtonBox::accepted, _dialog, &QDialog::accept);
    qApp->connect(buttonBox, &QDialogButtonBox::rejected, _dialog, &QDialog::reject);
    if (_connectOkToContentApply)
        qApp->connect(buttonBox, SIGNAL(accepted()), _content, SLOT(apply()));
    if (!_helpTopic.isEmpty()) // TODO process help
        qApp->connect(buttonBox, &QDialogButtonBox::helpRequested, [this](){
            info(QString("TODO help by topic '%1'").arg(this->_helpTopic));
        });

    QVBoxLayout* dialogLayout = new QVBoxLayout(_dialog);
    if (!_prompt.isEmpty())
    {
        QBoxLayout *promptLayout;
        if (_isPromptVertical) promptLayout = new QVBoxLayout;
        else promptLayout = new QHBoxLayout;
        promptLayout->setMargin(0);
        promptLayout->addWidget(new QLabel(_prompt));
        promptLayout->addWidget(_content);
        dialogLayout->addLayout(promptLayout);
        _contentLayout = promptLayout;
    }
    else
    {
        dialogLayout->addWidget(_content);
        _contentLayout = dialogLayout;
    }
    if (_fixedContentSize)
        dialogLayout->addStretch();
    if (_contentToButtonsSpacingFactor > 1)
    {
        int defaultSpacing = qApp->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
        dialogLayout->addSpacing(defaultSpacing * (_contentToButtonsSpacingFactor - 1));
    }
    dialogLayout->addWidget(buttonBox);

    setDlgTitle(_dialog, _title);
    setDlgIcon(_dialog, _iconPath);
}

} // namespace Dlg
} // namespace Ori
