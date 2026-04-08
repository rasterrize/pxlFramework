#pragma once

#include <glm/common.hpp>

#include "BufferLayout.h"

namespace pxl
{
    struct TexturedVertex
    {
        // Position of vertex in normalized device coordinates
        glm::vec3 Position = glm::vec3(0.0f);

        // Colour of the vertex. Passed to the fragment shader
        glm::vec4 Colour = glm::vec4(1.0f);

        // UV coords to use when sampling the texture
        glm::vec2 TexCoords = glm::vec2(0.0f);

        // The texture slot to sample from in the fragment shader
        uint32_t TexIndex = 0;

        static BufferLayout GetLayout()
        {
            BufferLayout layout;
            layout.Add(BufferDataType::Vec3); // vertex position
            layout.Add(BufferDataType::Vec4); // colour
            layout.Add(BufferDataType::Vec2); // texture coords
            layout.Add(BufferDataType::Int);  // texture slot index

            return layout;
        }
    };

    struct ColouredVertex
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Colour = glm::vec4(1.0f);

        static BufferLayout GetLayout()
        {
            BufferLayout layout;
            layout.Add(BufferDataType::Vec3); // Position
            layout.Add(BufferDataType::Vec4); // Colour

            return layout;
        }
    };
}