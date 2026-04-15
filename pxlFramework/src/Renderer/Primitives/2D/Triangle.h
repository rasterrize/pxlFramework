#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Primitive.h"
#include "Renderer/Texture.h"
#include "Renderer/Vertices.h"

namespace pxl
{
    struct Triangle
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec2 Size = glm::vec3(1.0f);
        glm::vec4 Colour = glm::vec4(1.0f);

        static constexpr std::array<TexturedVertex, 3> GetDefaultVertices()
        {
            auto texCoords = GetDefaultTexCoords();
            return {
                // clang-format off
                TexturedVertex({  0.0f,  0.5f, 0.0f }, glm::vec4(1.0f), texCoords[0]), 
                TexturedVertex({ -0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), texCoords[1]),
                TexturedVertex({  0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), texCoords[2]),
            }; // clang-format on
        }

        static constexpr std::array<uint32_t, 3> GetDefaultIndices()
        {
            return { 0, 1, 2 };
        }

        static constexpr std::array<glm::vec2, 3> GetDefaultTexCoords()
        {
            // TODO: sort out texture UVs
            return {
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
            };
        }
    };
}