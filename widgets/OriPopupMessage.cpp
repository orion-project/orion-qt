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

void PopupMessage::setTarget(QWidget *target)
{
    _target = target;
}

#define GET_TARGET \
    auto target = _target ? _target.get() : qApp->activeWindow(); \
    if (!target) { \
        qWarning() << "PopupMessage: explicit target window is not specified and app is not active"; \
        return; \
    }

void PopupMessage::warning(const QString& text, int duration)
{
    GET_TARGET
    (new PopupMessage(WARNING, text, duration, Qt::AlignHCenter|Qt::AlignVCenter, target))->show();
}

void PopupMessage::affirm(const QString& text, int duration)
{
    GET_TARGET
    (new PopupMessage(AFFIRM, text, duration, Qt::AlignHCenter|Qt::AlignVCenter, target))->show();
}

void PopupMessage::error(const QString& text, int duration)
{
    GET_TARGET
    (new PopupMessage(ERROR, text, duration, Qt::AlignHCenter|Qt::AlignVCenter, target))->show();
}

void PopupMessage::warning(const QString& text, Qt::Alignment align, int duration)
{
    GET_TARGET
    (new PopupMessage(WARNING, text, duration, align, target))->show();
}

void PopupMessage::affirm(const QString& text, Qt::Alignment align, int duration)
{
    GET_TARGET
    (new PopupMessage(AFFIRM, text, duration, align, target))->show();
}

void PopupMessage::error(const QString& text, Qt::Alignment align, int duration)
{
    GET_TARGET
    (new PopupMessage(ERROR, text, duration, align, target))->show();
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

PopupMessage::PopupMessage(Mode mode, const QString& text, int duration, Qt::Alignment align, QWidget *parent) : QFrame(parent), _mode(mode)
{
    cancel();

    setObjectName("OriPopupMessage");
    setAttribute(Qt::WA_DeleteOnClose);
    setProperty("mode", mode == WARNING ? "warning" : mode == AFFIRM ?  "affirm" : "error");
    setFrameShape(QFrame::NoFrame);

    auto label = new Ori::Widgets::Label(text);
    label->setAlignment(Qt::AlignHCenter);
    connect(label, &Ori::Widgets::Label::clicked, this, &PopupMessage::close);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(label);

    auto shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setOffset(2);
    setGraphicsEffect(shadow);

    adjustSize();
    auto sz = size();
    auto psz = parent->size();

    int x = align.testFlag(Qt::AlignLeft) ? windowMargin
        : align.testFlag(Qt::AlignRight) ? psz.width() - sz.width() - windowMargin
        : (psz.width() - sz.width())/2;
    int y = align.testFlag(Qt::AlignTop) ? windowMargin
        : align.testFlag(Qt::AlignBottom) ? psz.height() - sz.height() - windowMargin
        : (psz.height() - sz.height())/2;
    move(x, y);

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
