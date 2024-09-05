#pragma once

#include <glm/glm.hpp>

#include "Primitive.h"
#include "Renderer/Vertices.h"

namespace pxl
{
    struct Quad
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec2 Size = glm::vec3(1.0f);
        glm::vec4 Colour = glm::vec4(1.0f);
        Origin2D Origin = Origin2D::Center;

        glm::vec3 GetPositionWithOrigin() const
        {
            auto position = Position;
            switch (Origin)
            {
                case Origin2D::TopLeft:
                    position.x += Size.x / 2.0f;
                    position.y -= Size.y / 2.0f;
                    return position;
                case Origin2D::TopRight:
                    position.x -= Size.x / 2.0f;
                    position.y -= Size.y / 2.0f;
                    return position;
                case Origin2D::BottomLeft:
                    position.x += Size.x / 2.0f;
                    position.y += Size.y / 2.0f;
                    return position;
                case Origin2D::BottomRight:
                    position.x -= Size.x / 2.0f;
                    position.y += Size.y / 2.0f;
                    return position;
                case Origin2D::Center: return position;
            }

            return position;
        }

        bool Contains(const glm::vec2& position)
        {
            auto newPosition = position;

            // Move position opposite of origin
            switch (Origin)
            {
                case Origin2D::TopLeft:
                    newPosition.x += Size.x / 2.0f;
                    newPosition.y -= Size.y / 2.0f;
                    break;
                case Origin2D::TopRight:
                    newPosition.x -= Size.x / 2.0f;
                    newPosition.y -= Size.y / 2.0f;
                    break;
                case Origin2D::BottomLeft:
                    newPosition.x += Size.x / 2.0f;
                    newPosition.y += Size.y / 2.0f;
                    break;
                case Origin2D::BottomRight:
                    newPosition.x -= Size.x / 2.0f;
                    newPosition.y += Size.y / 2.0f;
                    break;
                case Origin2D::Center: break;
            }

            auto left = Position.x - Size.x / 2.0f;
            auto right = Position.x + Size.x / 2.0f;
            auto bottom = Position.y - Size.y / 2.0f;
            auto top = Position.y + Size.y / 2.0f;

            if (newPosition.x >= left && newPosition.x <= right && newPosition.y >= bottom && newPosition.y <= top)
                return true;

            return false;
        }

        static constexpr std::array<QuadVertex, 4> GetDefaultVertices()
        {
            return {
                QuadVertex({ -0.5f,  0.5f, 0.0f }, glm::vec4(1.0f), { 0.0f, 1.0f }), 
                QuadVertex({ -0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), { 0.0f, 0.0f }),
                QuadVertex({  0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), { 1.0f, 0.0f }),
                QuadVertex({  0.5f,  0.5f, 0.0f }, glm::vec4(1.0f), { 1.0f, 1.0f }),
            };
        }

        static constexpr std::array<uint32_t, 6> GetDefaultIndices()
        {
            return { 0, 1, 2, 2, 3, 0 };
        }
    };
}