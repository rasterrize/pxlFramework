#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "BufferLayout.h"

namespace pxl
{
    struct TexturedVertex
    {
        glm::vec3 Position = glm::vec3(0.0f);  // Position of vertex in NDC
        glm::vec4 Colour = glm::vec4(1.0f);    // Colour of the vertex. Passed to the fragment shader
        glm::vec2 TexCoords = glm::vec2(0.0f); // UV coords to use when sampling the TexIndex slot texture
        float TexIndex = 0.0f;                 // The texture slot to sample from in the fragment shader

        static constexpr BufferLayout GetLayout()
        {
            BufferLayout layout;
            layout.Add(BufferDataType::Vec3);  // vertex position
            layout.Add(BufferDataType::Vec4);  // colour
            layout.Add(BufferDataType::Vec2);  // texture coords
            layout.Add(BufferDataType::Float); // texture slot index

            return layout;
        }
    };

    struct ColouredVertex
    {
        glm::vec3 Position = glm::vec3(0.0f); // Position of vertex in NDC
        glm::vec4 Colour = glm::vec4(1.0f);   // Colour of the vertex. Passed to the fragment shader

        static constexpr BufferLayout GetLayout()
        {
            BufferLayout layout;
            layout.Add(BufferDataType::Vec3); // vertex position
            layout.Add(BufferDataType::Vec4); // colour

            return layout;
        }
    };

    struct LineVertex
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Colour = glm::vec4(1.0f);

        static constexpr BufferLayout GetLayout()
        {
            BufferLayout layout;
            layout.Add(BufferDataType::Vec3); // vertex position
            layout.Add(BufferDataType::Vec4); // colour

            return layout;
        }
    };

}