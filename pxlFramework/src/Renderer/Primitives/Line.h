#pragma once

#include <glm/glm.hpp>

namespace pxl
{
    struct Line
    {
        glm::vec3 StartPosition = glm::vec3(0.0f);
        glm::vec3 EndPosition = glm::vec3(1.0f);
        glm::vec3 Rotation = glm::vec3(1.0f);
        glm::vec4 Colour = glm::vec4(1.0f);

        float GetLength()
        {
            return 0.0f;
        }
    };
}