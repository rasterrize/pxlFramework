#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
            return BufferLayout({
                BufferDataType::Vec3, // vertex position
                BufferDataType::Vec4, // colour
                BufferDataType::Vec2, // texture coords
                BufferDataType::Int,  // texture slot index
            });
        }
    };

    struct ColouredVertex
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Colour = glm::vec4(1.0f);

        static BufferLayout GetLayout()
        {
            return BufferLayout({
                BufferDataType::Vec3, // vertex position
                BufferDataType::Vec4, // colour
            });
        }
    };
}