#pragma once

#include <glm/glm.hpp>

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
        std::optional<std::shared_ptr<Texture>> Texture;
        std::optional<std::array<glm::vec2, 4>> TextureUV;

        // bool Contains(const glm::vec2& position)
        // {
        //     auto newPosition = position;

        //     auto left = Position.x - Size.x / 2.0f;
        //     auto right = Position.x + Size.x / 2.0f;
        //     auto bottom = Position.y - Size.y / 2.0f;
        //     auto top = Position.y + Size.y / 2.0f;

        //     return newPosition.x >= left && newPosition.x <= right && newPosition.y >= bottom && newPosition.y <= top;
        // }

        static constexpr std::array<QuadVertex, 3> GetDefaultVertices()
        {
            auto texCoords = GetDefaultTexCoords();
            return {
                // clang-format off
                QuadVertex({  0.0f,  0.5f, 0.0f }, glm::vec4(1.0f), texCoords[0]), 
                QuadVertex({ -0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), texCoords[1]),
                QuadVertex({  0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), texCoords[2]),
            }; // clang-format on
        }
        
        static constexpr std::array<uint32_t, 6> GetDefaultIndices()
        {
            return { 0, 1, 2 };
        }
        
        static constexpr std::array<glm::vec2, 4> GetDefaultTexCoords()
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