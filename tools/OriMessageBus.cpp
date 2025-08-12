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
    instance().sendEvent(event, params);
}

void MessageBus::registerListener(IMessageBusListener *listener)
{
    if (_isSending) {
        _pendingRegistrations << listener;
        return;
    }
    if (!_listeners.contains(listener))
        _listeners.push_back(listener);
}

void MessageBus::unregisterListener(IMessageBusListener *listener)
{
    if (_isSending) {
        _pendingUnregistrations << listener;
        return;
    }
    _listeners.removeOne(listener);
}

void MessageBus::sendEvent(int event, const QMap<QString, QVariant>& params)
{
    _isSending = true;
    for (auto listener : std::as_const(_listeners))
        listener->messageBusEvent(event, params);
    _isSending = false;
    
    if (!_pendingRegistrations.isEmpty()) {
        for (auto listener : std::as_const(_pendingRegistrations))
            registerListener(listener);
        _pendingRegistrations.clear();
    }
    if (!_pendingUnregistrations.isEmpty()) {
        for (auto listener : std::as_const(_pendingUnregistrations))
            unregisterListener(listener);
        _pendingUnregistrations.clear();
    }
}
    
} // namespace Ori
