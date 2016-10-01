#include "ColorPreview.h"
#include "helpers/OriTools.h"
#include "helpers/OriWidgets.h"

ColorPreview::ColorPreview(QWidget *parent) : QWidget(parent)
{
    Ori::Gui::layoutV(this, 0, 0, {
        _preview  = new QFrame,
        Ori::Gui::defaultSpacing(),
        _textHtml = makeText(),
        _textRgb = makeText()
    });

    _preview->setFrameStyle(QFrame::Sunken | QFrame::Panel);
    _preview->setAutoFillBackground(true);
    _preview->setBackgroundRole(QPalette::Base);
    _preview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
}

void ColorPreview::setColor(const QColor& color)
{
    _color = color;

    QPalette p;
    p.setColor(QPalette::Base, color);
    _preview->setPalette(p);

    _textHtml->setText(Ori::Color::formatHtml(_color));
    _textRgb->setText(Ori::Color::formatRgb(_color));
}

QLineEdit* ColorPreview::makeText()
{
    auto text = new QLineEdit;
    text->setReadOnly(true);
    QPalette p;
    p.setColor(QPalette::Base, p.color(QPalette::Window));
    text->setPalette(p);
    return text;
}
