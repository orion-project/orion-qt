#ifndef ORI_POPUP_MESSAGE_H
#define ORI_POPUP_MESSAGE_H

#include <QFrame>

namespace Ori::Gui {

/**
Temporary popup message, automatically hidden by timeout.

Pass zero duration to disable auto-hide behaviour, then use `cancel()` to close the message.

Usage:

```cpp
#include "widgets/OriPopupMessage.h"

Ori::Gui::PopupMessage::affirm("Image has been copied to Clipboard");
```

The message is transparent by default. To give some look use application styles:

For example:

```qss
#OriPopupMessage {
  border-radius: 4px;
}
#OriPopupMessage QLabel {
  font-size: 10pt;
  margin: 15px;
}
#OriPopupMessage[mode=warning] {
  border: 1px solid #f3af8f;
  background: #ffdcbc;
}
#OriPopupMessage[mode=affirm] {
  border: 1px solid #7ee87e;
  background: #b5fbb5;
}
#OriPopupMessage[mode=error] {
  border: 1px solid #e87e7e;
  background: #ffb5b5;
}
#OriPopupMessage[mode=hint] {
  border: 1px solid #e5c300;
  background: #ffec7c;
}
```
*/

class PopupMessage : public QFrame
{
    Q_OBJECT

public:
    // There seems some undesirable def goes into scope in older Qt
    // (<= 6.2; it's ok for 6.7+, not's sure what's in between)
    #ifdef ERROR
        #undef ERROR
    #endif
    enum Mode {WARNING, AFFIRM, ERROR, HINT};

    struct Options
    {
        Mode mode = AFFIRM;
        QString text;
        int duration = -1;
        Qt::Alignment align = Qt::AlignHCenter|Qt::AlignVCenter;
        Qt::Alignment textAlign = Qt::AlignHCenter;
        QPixmap pixmap;
    };

    static void setTarget(QWidget *target);
    static void warning(const QString& text, int duration = -1);
    static void affirm(const QString& text, int duration = -1);
    static void error(const QString& text, int duration = -1);
    static void hint(const QString& text, int duration = -1);
    static void warning(const QString& text, Qt::Alignment align, int duration = -1);
    static void affirm(const QString& text, Qt::Alignment align, int duration = -1);
    static void error(const QString& text, Qt::Alignment align, int duration = -1);
    static void hint(const QString& text, Qt::Alignment align, int duration = -1);
    static void show(const Options &opts, QWidget *parent = nullptr);
    static void cancel();

    static int windowMargin;
    static int defaultDuration;

    ~PopupMessage();

protected:
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    explicit PopupMessage(const Options &opts, QWidget *patent);

    static PopupMessage* _instance;
    static QPointer<QWidget> _target;
};

} // namespace Ori::Gui

#endif // ORI_POPUP_MESSAGE_H
