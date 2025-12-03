#pragma once

#include "Event.h"
#include "IEventHandler.h"

namespace pxl
{
    class EventManager
    {
    public:
        std::function<void(Event&)> GetEventSendCallback();
        std::function<void(std::unique_ptr<Event> e)> GetEventQueueCallback();

        void RegisterHandler(const std::shared_ptr<IEventHandler>& handler);

    private:
        friend class Application;
        void ProcessQueue();

        void HandleEvent(Event& e);

    private:
        std::vector<std::unique_ptr<Event>> m_EventQueue;

        std::vector<std::weak_ptr<IEventHandler>> m_Handlers;
    };
}