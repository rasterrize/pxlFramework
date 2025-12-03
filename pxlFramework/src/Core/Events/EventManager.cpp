#include "EventManager.h"

#include "Core/Application.h"
#include "KeyboardEvents.h"

namespace pxl
{
    std::function<void(Event&)> EventManager::GetEventSendCallback()
    {
        return [this](Event& e)
        {
            HandleEvent(e);
        };
    }

    std::function<void(std::unique_ptr<Event> e)> EventManager::GetEventQueueCallback()
    {
        return [this](std::unique_ptr<Event> e)
        {
            m_EventQueue.push_back(std::move(e));
        };
    }

    void EventManager::RegisterHandler(const std::shared_ptr<IEventHandler>& handler)
    {
        m_Handlers.emplace_back(handler);
    }

    void EventManager::ProcessQueue()
    {
        PXL_PROFILE_SCOPE;

        for (auto& event : m_EventQueue)
            HandleEvent(*event);

        m_EventQueue.clear();
    }

    void EventManager::HandleEvent(Event& e)
    {
        PXL_PROFILE_SCOPE;

        // Prioritize core application events first
        Application::Get().OnEvent(e);

        if (e.IsHandled())
            return;

        // TODO: Handle events for UI layers

        // Handle events for event handlers
        for (int i = 0; i < m_Handlers.size(); i++)
        {
            auto& handler = m_Handlers.at(i);

            if (handler.expired())
            {
                m_Handlers.erase(m_Handlers.begin() + i);
                continue;
            }

            handler.lock()->OnEvent(e);

            if (e.IsHandled())
                return;
        }
    }
}