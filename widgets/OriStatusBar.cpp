#include "OriStatusBar.h"
#include "OriLabels.h"

#include <QApplication>
#include <QIcon>

namespace Ori {
namespace Widgets {

StatusBar::StatusBar(int count, QWidget *parent) : QStatusBar(parent)
{
    _iconSize = QSize(16, 16);

    for (int i = 0; i < count; i++)
    {
        auto label = new Label;
        label->setContentsMargins(6, 0, 6, 0);
        label->setContextMenuPolicy(Qt::CustomContextMenu);
        _sections.push_back(label);
        addWidget(label);
    }
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
}

void StatusBar::setText(int index, const QString &text)
{
    _sections[index]->setText(text);
}

void StatusBar::setText(int index, const QString& text, const QString& tooltip)
{
    setText(index, text);
    setHint(index, tooltip);
}

void StatusBar::setIcon(int index, const QString &path)
{
    _sections[index]->setPixmap(QIcon(path).pixmap(_iconSize));
}

void StatusBar::setToolTip(int index, const QString& tooltip)
{
    _sections[index]->setToolTip(tooltip);
}

void StatusBar::setMargin(int index, int left, int right)
{
    _sections[index]->setContentsMargins(left, 0, right, 0);
}

void StatusBar::setVisible(int index, bool on)
{
    _sections[index]->setVisible(on);
}

void StatusBar::clear(int index)
{
    setText(index, QString());
    setHint(index, QString());
    setIcon(index, QString());
    highlightReset(index);
}

void StatusBar::clear()
{
    for (int i = 0; i < _sections.size(); i++) clear(i);
}

void StatusBar::connect(int index, const char *signal, const QObject *receiver, const char *method)
{
    QStatusBar::connect(_sections[index], signal, receiver, method);
}

void StatusBar::highlightError(int index)
{
    _sections[index]->setStyleSheet("QLabel{background:red;color:white;font-weight:bold}");
}

void StatusBar::highlightReset(int index)
{
    _sections[index]->setStyleSheet(QString());
}

void StatusBar::setIconSize(const QSize& size)
{
    _iconSize = size;
}

QPoint StatusBar::mapToGlobal(int index, const QPoint& p)
{
    return _sections[index]->mapToGlobal(p);
}

Label *StatusBar::addVersionLabel()
{
    auto label = new Label(qApp->applicationVersion());
    label->setContentsMargins(3, 0, 3, 0);
    addPermanentWidget(label);
    return label;
}

} // namespace Widgets
} // namespace Ori
