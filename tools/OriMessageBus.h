#ifndef ORI_MESSAGE_BUS_H
#define ORI_MESSAGE_BUS_H

#include "../core/OriTemplates.h"

#include <QVariant>

namespace Ori {

class IMessageBusListener
{
public:
    IMessageBusListener();
    ~IMessageBusListener();

    virtual void messageBusEvent(int event, const QMap<QString, QVariant>& params) = 0;
};

/**

Simple message bus implementation.

Example of usage:

```cpp
#include "tools/OriMessageBus.h"

#define MSG_SOMETHING_HAPPENED 1

class MainWindow : public QMainWindow, public Ori::IMessageBusListener
{
public:
    MainWindow() : QMainWindow() {
        Ori::MessageBus::instance().registerListener(this);
    }

    void messageBusEvent(int event, const QMap<QString, QVariant>& params) override {
        switch (event) {
        case MSG_SOMETHING_HAPPENED:
            QString id = params.value("id").toString();
            qDebug() << "MSG_SOMETHING_HAPPENED" << id;
            break;
        }
    }
};

void AnotherWindow::doSomething() {
    QString id = "Something has been done to this object";
    Ori::MessageBus::send(MSG_SOMETHING_HAPPENED, {{"id", id}});
}
```

*/
class MessageBus :
    public Singleton<MessageBus>,
    public Notifier<IMessageBusListener>
{
public:
    static void send(int event, const QMap<QString, QVariant>& params = {});

private:
    MessageBus() {}

    friend class Singleton<MessageBus>;
};

} // namespace Ori

#endif // ORI_MESSAGE_BUS_H
