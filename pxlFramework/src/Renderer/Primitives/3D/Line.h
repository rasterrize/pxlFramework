#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace pxl
{
    struct Line
    {
        glm::vec3 StartPosition = glm::vec3(0.0f);
        glm::vec3 EndPosition = glm::vec3(0.0f);
        glm::vec4 Colour = glm::vec4(1.0f);

        float Length() { return glm::length(StartPosition - EndPosition); }
    };
}