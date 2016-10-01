#include "OriInfoPanel.h"

#include <QBoxLayout>
#include <QLabel>

namespace Ori {
namespace Widgets {

InfoPanel::InfoPanel(QWidget *parent) : QFrame(parent)
{
    ///////// description layout
    QLabel *labelImage = new QLabel;
    labelImage->setPixmap(QPixmap(":/toolbar/info")); // TODO extract to orion images
    labelImage->setMaximumWidth(24);
    labelImage->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    labelDescr = new QLabel;
    labelDescr->setWordWrap(true);
    labelDescr->setAlignment(Qt::AlignVCenter);
    labelDescr->setForegroundRole(QPalette::Text);

    QHBoxLayout *layoutDescr = new QHBoxLayout;
    layoutDescr->addWidget(labelImage);
    layoutDescr->addWidget(labelDescr);

    ///////// description frame
    setFrameStyle(QFrame::Sunken | QFrame::Panel);
    setBackgroundRole(QPalette::ToolTipBase);
    setAutoFillBackground(true);
    setLayout(layoutDescr);
}

void InfoPanel::setInfo(const QString& text)
{
    labelDescr->setText(text);
}

} // namespace Widgets
} // namespace Ori

