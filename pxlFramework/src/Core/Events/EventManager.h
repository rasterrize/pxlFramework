#pragma once

#include "Event.h"
#include "IEventHandler.h"

namespace pxl
{
    class EventManager
    {
    public:
        EventManager();

        std::function<void(Event&)> GetEventSendCallback() const { return m_EventSendCallback; }
        std::function<void(std::unique_ptr<Event>)> GetEventQueueCallback() const { return m_EventQueueCallback; }

        void RegisterHandler(const std::shared_ptr<IEventHandler> handler);

    private:
        friend class Application;
        void ProcessQueue();

        void HandleEvent(Event& e);

    private:
        std::vector<std::unique_ptr<Event>> m_EventQueue;
        std::vector<std::weak_ptr<IEventHandler>> m_Handlers;

        std::function<void(Event&)> m_EventSendCallback;
        std::function<void(std::unique_ptr<Event>)> m_EventQueueCallback;
    };
}