#pragma once

#include <glm/glm.hpp>

#include "Primitive.h"

#include "Renderer/Vertices.h"

namespace pxl
{
    struct Quad
    {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec2 Size;
        glm::vec4 Colour;
        Origin Origin;

        bool Contains(const glm::vec2& position)
        {
            auto newPosition = position;
            
            // Move position opposite of origin
            switch (Origin)
            {
                case Origin::TopLeft:
                    newPosition.x += Size.x / 2.0f;
                    newPosition.y -= Size.y / 2.0f;
                    break;
                case Origin::TopRight:
                    newPosition.x -= Size.x / 2.0f;
                    newPosition.y -= Size.y / 2.0f;
                    break;
                case Origin::BottomLeft:
                    newPosition.x += Size.x / 2.0f;
                    newPosition.y += Size.y / 2.0f;
                    break;
                case Origin::BottomRight:
                    newPosition.x -= Size.x / 2.0f;
                    newPosition.y += Size.y / 2.0f;
                    break;
                case Origin::Center: break;
            }

            auto left = Position.x - Size.x / 2.0f;
            auto right = Position.x + Size.x / 2.0f;
            auto bottom = Position.y - Size.y / 2.0f;
            auto top = Position.y + Size.y / 2.0f;

            if (newPosition.x >= left && newPosition.x <= right && newPosition.y >= bottom && newPosition.y <= top)
                return true;
            
            return false;
        }

        constexpr static std::array<QuadVertex, 4> GetDefaultVertices()
        {
            return {
                QuadVertex({ -0.5f,  0.5f, 0.0f }, glm::vec4(1.0f), { 0.0f, 1.0f }), 
                QuadVertex({ -0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), { 0.0f, 0.0f }),
                QuadVertex({  0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), { 1.0f, 0.0f }),
                QuadVertex({  0.5f,  0.5f, 0.0f }, glm::vec4(1.0f), { 1.0f, 1.0f }),
            };
        }

        constexpr static std::array<uint32_t, 6> GetDefaultIndices()
        {
            return { 0, 1, 2, 2, 3, 0 };
        }
    };
}