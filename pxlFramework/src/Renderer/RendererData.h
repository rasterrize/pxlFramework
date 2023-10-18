#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    struct TriangleVertex
    {
        glm::vec3 Position  = glm::vec3(0.0f);
        glm::vec4 Colour    = glm::vec4(1.0f);
        glm::vec2 TexCoords = glm::vec2(0.0f);
    };

    struct LineVertex
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Colour   = glm::vec4(1.0f);
    };

    struct Mesh
    {
        std::vector<TriangleVertex> Vertices;
        std::vector<uint32_t> Indices;

        glm::mat4 Transform = glm::mat4(0.0f);

        void Translate(float x, float y, float z)
        {
            Transform = glm::translate(glm::mat4(1.0f), { x, y, z });
        }
    };
}