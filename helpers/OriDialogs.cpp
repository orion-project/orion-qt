#include "OriDialogs.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
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
/*
Dialog::Dialog(QWidget *content, bool ownContent) : _content(content), _isOwnContent(ownContent)
{
    _dialog = new QDialog(qApp->activeWindow());
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    qApp->connect(buttons, &QDialogButtonBox::accepted, _dialog, &QDialog::accept);
    qApp->connect(buttons, &QDialogButtonBox::rejected, _dialog, &QDialog::reject);
    _layout = new QVBoxLayout(_dialog);
    _layout->addWidget(content);
    _layout->addWidget(buttons);
}

Dialog::~Dialog()
{
    if (!_isOwnContent)
        _layout->removeWidget(_content);
    delete _dialog;
}

Dialog& Dialog::setTitle(const QString& title)
{
    _dialog->setWindowTitle(title);
    _isTitleSet = true;
    return *this;
}

bool Dialog::exec()
{
    if (!_isTitleSet)
        _dialog->setWindowTitle(qApp->applicationName());
    return _dialog->exec() == QDialog::Accepted;
}
*/
} // namespace Dlg
} // namespace Ori
