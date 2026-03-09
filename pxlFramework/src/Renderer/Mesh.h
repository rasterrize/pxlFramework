#pragma once

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

        Mesh(const std::vector<TexturedVertex>& vertices, const std::vector<uint32_t>& indices)
            : Vertices(vertices), Indices(indices)
        {
        }

        std::vector<TexturedVertex> Vertices;
        std::vector<uint32_t> Indices;
    };
}