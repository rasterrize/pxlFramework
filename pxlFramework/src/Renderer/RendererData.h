#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "BufferLayout.h"
#include "Vertices.h"

namespace pxl
{
    struct Mesh
    {
        Mesh() = delete;
        Mesh(const Mesh& other) = delete;
        Mesh(uint32_t vertexCount, uint32_t indexCount)
        {
            Vertices.reserve(vertexCount);
            Indices.reserve(indexCount);
        }

        Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
            : Vertices(vertices), Indices(indices)
        {
        }

        std::vector<MeshVertex> Vertices;
        std::vector<uint32_t> Indices;
    };
}