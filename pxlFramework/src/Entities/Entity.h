#pragma once

#include "Components/Component.h"

namespace pxl
{
    template<typename T>
    class Entity
    {
    public:
        // Component GetComponent<T>() { return m_Components[T]; }
    private:
        std::unordered_map<T, Component> m_Components;
    };
}