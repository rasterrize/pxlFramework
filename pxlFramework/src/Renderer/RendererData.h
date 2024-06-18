#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "BufferLayout.h"

namespace pxl
{
    struct QuadVertex
    {
        glm::vec3 Position  = glm::vec3(0.0f); // Position of vertex in (NDC??)
        glm::vec4 Colour    = glm::vec4(1.0f); // Colour of the vertex. Passed to the fragment shader
        glm::vec2 TexCoords = glm::vec2(0.0f); // UV coords to use when sampling the TexIndex slot texture
        float TexIndex = -1.0f;                // The texture slot to sample from in the fragment shader

        static BufferLayout GetLayout()
        {
            BufferLayout layout;
            layout.Add({ BufferDataType::Float3, false }); // vertex position
            layout.Add({ BufferDataType::Float4, false }); // colour
            layout.Add({ BufferDataType::Float2, false }); // texture coords
            layout.Add({ BufferDataType::Float, false });  // texture slot index

            return layout;
        }
    };

    struct LineVertex
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Colour   = glm::vec4(1.0f);
    };

    struct Mesh
    {
        std::vector<QuadVertex> Vertices;
        std::vector<uint32_t> Indices;

        glm::mat4 Transform = glm::mat4(0.0f);

        void Translate(float x, float y, float z)
        {
            Transform = glm::translate(glm::mat4(1.0f), { x, y, z });
        }
    };
}