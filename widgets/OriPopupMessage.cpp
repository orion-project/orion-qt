#include "OriPopupMessage.h"
#include "OriLabels.h"

#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTimer>

namespace Ori::Gui {

void PopupMessage::setTarget(QWidget *target) {
    _target = target;
}

void PopupMessage::warning(const QString& text, int duration) {
    show({.mode=WARNING, .text=text, .duration=duration, .align=Qt::AlignHCenter|Qt::AlignVCenter});
}

void PopupMessage::affirm(const QString& text, int duration) {
    show({.mode=AFFIRM, .text=text, .duration=duration, .align=Qt::AlignHCenter|Qt::AlignVCenter});
}

void PopupMessage::error(const QString& text, int duration) {
    show({.mode=ERROR, .text=text, .duration=duration, .align=Qt::AlignHCenter|Qt::AlignVCenter});
}

void PopupMessage::hint(const QString& text, int duration) {
    show({.mode=HINT, .text=text, .duration=duration, .align=Qt::AlignHCenter|Qt::AlignVCenter});
}

void PopupMessage::warning(const QString& text, Qt::Alignment align, int duration) {
    show({.mode=WARNING, .text=text, .duration=duration, .align=align});
}

void PopupMessage::affirm(const QString& text, Qt::Alignment align, int duration) {
    show({.mode=AFFIRM, .text=text, .duration=duration, .align=align});
}

void PopupMessage::error(const QString& text, Qt::Alignment align, int duration) {
    show({.mode=ERROR, .text=text, .duration=duration, .align=align});
}

void PopupMessage::hint(const QString& text, Qt::Alignment align, int duration) {
    show({.mode=HINT, .text=text, .duration=duration, .align=align});
}

void PopupMessage::show(const Options &opts, QWidget *parent)
{
    auto target = parent ? parent : _target ? _target.get() : qApp->activeWindow();
    if (!target) {
        qWarning() << "PopupMessage: explicit target window is not specified and app is not active";
        return;
    }
    QWidget *wnd = new PopupMessage(opts, target);
    wnd->show();
}

void PopupMessage::cancel()
{
    if (_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

int PopupMessage::windowMargin = 20;
int PopupMessage::defaultDuration = 2000;
PopupMessage* PopupMessage::_instance = nullptr;
QPointer<QWidget> PopupMessage::_target;

PopupMessage::PopupMessage(const Options &opts, QWidget *parent) : QFrame(parent)
{
    cancel();

    setObjectName("OriPopupMessage");
    setAttribute(Qt::WA_DeleteOnClose);
    setProperty("mode", opts.mode == WARNING ? "warning" :
                        opts.mode == AFFIRM ?  "affirm" :
                        opts.mode == ERROR ? "error" : "hint");
    setFrameShape(QFrame::NoFrame);

    auto layout = new QHBoxLayout(this);

    if (!opts.pixmap.isNull()) {
        auto label = new QLabel;
        label->setPixmap(opts.pixmap);
        layout->addWidget(label);
    }

    auto label = new Ori::Widgets::Label(opts.text);
    label->setAlignment(opts.textAlign);
    connect(label, &Ori::Widgets::Label::clicked, this, &PopupMessage::close);
    layout->addWidget(label);

    auto shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setOffset(2);
    setGraphicsEffect(shadow);

    adjustSize();
    auto sz = size();
    auto psz = parent->size();

    int x = opts.align.testFlag(Qt::AlignLeft) ? windowMargin
        : opts.align.testFlag(Qt::AlignRight) ? psz.width() - sz.width() - windowMargin
        : (psz.width() - sz.width())/2;
    int y = opts.align.testFlag(Qt::AlignTop) ? windowMargin
        : opts.align.testFlag(Qt::AlignBottom) ? psz.height() - sz.height() - windowMargin
        : (psz.height() - sz.height())/2;
    move(x, y);

    int duration = opts.duration;
    if (duration < 0)
        duration = defaultDuration;
    if (duration > 0)
        QTimer::singleShot(duration, this, [this](){
            auto opacity = new QGraphicsOpacityEffect();
            setGraphicsEffect(opacity);
            auto fadeout = new QPropertyAnimation(opacity, "opacity");
            fadeout->setDuration(1000);
            fadeout->setStartValue(1);
            fadeout->setEndValue(0);
            fadeout->setEasingCurve(QEasingCurve::OutBack);
            fadeout->start(QPropertyAnimation::DeleteWhenStopped);
            connect(fadeout, &QPropertyAnimation::finished, this, &PopupMessage::close);
        });

    _instance = this;
}

PopupMessage::~PopupMessage()
{
    _instance = nullptr;
}

void PopupMessage::mouseReleaseEvent(QMouseEvent*)
{
    close();
}

} // namespace Ori::Gui
