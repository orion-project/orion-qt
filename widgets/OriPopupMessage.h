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
```
*/

class PopupMessage : public QFrame
{
    Q_OBJECT

public:
    enum Mode {WARNING, AFFIRM, ERROR};

    static void setTarget(QWidget *target);
    static void warning(const QString& text, int duration = -1);
    static void affirm(const QString& text, int duration = -1);
    static void error(const QString& text, int duration = -1);
    static void warning(const QString& text, Qt::Alignment align, int duration = -1);
    static void affirm(const QString& text, Qt::Alignment align, int duration = -1);
    static void error(const QString& text, Qt::Alignment align, int duration = -1);

    static void cancel();

    static int windowMargin;
    static int defaultDuration;

    explicit PopupMessage(Mode mode, const QString& text, int duration, Qt::Alignment align, QWidget *parent);
    ~PopupMessage();

protected:
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    Mode _mode;
    static PopupMessage* _instance;
    static QPointer<QWidget> _target;
};

} // namespace Ori::Gui

#endif // ORI_POPUP_MESSAGE_H
