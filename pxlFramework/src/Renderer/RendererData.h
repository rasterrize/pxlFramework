#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "BufferLayout.h"
#include "Vertices.h"

namespace pxl
{
    struct Mesh
    {
        std::vector<MeshVertex> Vertices;
        std::vector<uint32_t> Indices;

        glm::mat4 Transform = glm::mat4(0.0f);

        void Translate(float x, float y, float z)
        {
            Transform = glm::translate(glm::mat4(1.0f), { x, y, z });
        }
    };
}