#pragma once

#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// Thanks microsoft!
#undef RGB

namespace pxl
{
    namespace Colour
    {
        template<typename T>
        inline glm::vec3 RGB(T r, T g, T b)
        {
            return glm::clamp(glm::vec3(r, g, b) / glm::vec3(255.0f), 0.0f, 1.0f);
        }

        template<typename T>
        inline glm::vec4 RGBA(T r, T g, T b, T a)
        {
            return glm::clamp(glm::vec4(r, g, b, a) / glm::vec4(255.0f), 0.0f, 1.0f);
        }

        inline glm::vec4 White()
        {
            return glm::vec4(1.0f);
        }

        inline glm::vec4 Black()
        {
            return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        inline glm::vec4 Red()
        {
            return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        inline glm::vec4 Green()
        {
            return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        }

        inline glm::vec4 Blue()
        {
            return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        }

        inline glm::vec4 Pink()
        {
            return glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
        }

        inline glm::vec4 Yellow()
        {
            return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        }
    };
}