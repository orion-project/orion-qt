#include "OriMessageBus.h"

namespace Ori {

void MessageBus::send(int event, const QMap<QString, QVariant>& params)
{
    notify(&IMessageBusListener::messageBusEvent, event, params);
}

} // namespace Ori
