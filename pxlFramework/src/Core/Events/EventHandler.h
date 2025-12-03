#pragma once

#include "IEventHandler.h"

// Creates a new event handler supporting a non-static object function callback
#define PXL_CREATE_HANDLER(eventType, eventFunction) std::make_shared<pxl::EventHandler<eventType>>([this](auto& e) { eventFunction(e); });

// Creates a new event handler supporting a static function callback
#define PXL_CREATE_STATIC_HANDLER(eventType, eventFunction) std::make_shared<pxl::EventHandler<eventType>>([](auto& e) { eventFunction(e); });

// Registers an event handler already created using PXL_CREATE_HANDLER or PXL_CREATE_STATIC_HANDLER
#define PXL_REGISTER_EXISTING_HANDLER(handler) \
    pxl::Application::Get().GetEventManager()->RegisterHandler(handler);

// Registers a new event handles by creating and automatically registering it to the event system
#define PXL_REGISTER_HANDLER(handler, eventType, eventFunction) \
    pxl::Application::Get().GetEventManager()->RegisterHandler( \
    handler = std::make_shared<pxl::EventHandler<eventType>>([this](auto& e) { eventFunction(e); }));

// Registers a new static event handler by creating and automatically registering it to the event system
#define PXL_REGISTER_STATIC_HANDLER(handler, eventType, eventFunction) \
    pxl::Application::Get().GetEventManager()->RegisterHandler( \
    handler = std::make_shared<pxl::EventHandler<eventType>>([](auto& e) { eventFunction(e); }));

namespace pxl
{
    template<typename EventT>
    class EventHandler : public IEventHandler
    {
    public:
        EventHandler(const std::function<void(EventT&)>& func)
            : m_UserCallback(func)
        {
        }

        virtual void EventHandler::OnEvent(Event& e) override
        {
            PXL_PROFILE_SCOPE;

            if (e.GetType() == EventT::GetStaticType())
                m_UserCallback(static_cast<EventT&>(e));
        }

    private:
        std::function<void(EventT& e)> m_UserCallback = nullptr;
    };

    template<typename T>
    using UserEventHandler = std::shared_ptr<EventHandler<T>>;
}