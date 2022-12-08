#include "OriInfoPanel.h"

#include <QBoxLayout>
#include <QLabel>
#include <QIcon>

namespace Ori {
namespace Widgets {

InfoPanel::InfoPanel(QWidget *parent) : QFrame(parent)
{
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(QIcon(":/ori_images/info").pixmap(24, 24));
    labelImage->setMaximumWidth(24);
    labelImage->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _labelTitle = new QLabel;
    _labelTitle->setWordWrap(true);
    _labelTitle->setStyleSheet(QStringLiteral("font-weight:bold"));

    _labelDescr = new QLabel;
    _labelDescr->setWordWrap(true);
    _labelDescr->setForegroundRole(QPalette::Text);

    QVBoxLayout *layoutDescr = new QVBoxLayout;
    layoutDescr->setContentsMargins(0, 0, 0, 0);
    layoutDescr->setSpacing(6);
    layoutDescr->addWidget(_labelTitle);
    layoutDescr->addWidget(_labelDescr);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(labelImage);
    mainLayout->addLayout(layoutDescr);

    setFrameStyle(QFrame::StyledPanel);
    setBackgroundRole(QPalette::ToolTipBase);
    setAutoFillBackground(true);
}

void InfoPanel::setInfo(const QString &title, const QString& text)
{
    _labelTitle->setVisible(true);
    _labelTitle->setText(title);
    _labelDescr->setVisible(!text.isEmpty());
    _labelDescr->setText(text);
}

void InfoPanel::setInfo(const QString& text)
{
    _labelTitle->setVisible(false);
    _labelDescr->setText(text);
}

void InfoPanel::setMargin(int value)
{
    qobject_cast<QBoxLayout*>(layout())->setContentsMargins(value, value, value, value);
}

void InfoPanel::setSpacing(int value)
{
    qobject_cast<QBoxLayout*>(layout())->setSpacing(value);
}

} // namespace Widgets
} // namespace Ori

