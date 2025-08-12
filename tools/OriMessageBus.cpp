#include "OriMessageBus.h"

namespace Ori {

IMessageBusListener::IMessageBusListener()
{
    MessageBus::instance().registerListener(this);
}

IMessageBusListener::~IMessageBusListener()
{
    MessageBus::instance().unregisterListener(this);
}

void MessageBus::send(int event, const QMap<QString, QVariant>& params)
{
    instance().notify(&IMessageBusListener::messageBusEvent, event, params);
}

} // namespace Ori
