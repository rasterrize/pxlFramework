#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Renderer/Primitives/Primitive.h"
#include "Renderer/Vertices.h"

namespace pxl
{
    enum QuadFlags
    {
    };

    /// @brief A 2D 4-sided polygon with equal corners (aka, a scalable rectangle)
    struct Quad
    {
        glm::vec3 Position = glm::vec3(0.0f);
        float Rotation = 0.0f;
        glm::vec2 Size = glm::vec3(1.0f);
        glm::vec4 Colour = glm::vec4(1.0f);
        Origin2D Origin = Origin2D::Centre;
        Anchor2D Anchor = Anchor2D::None;
        Scaling2D Scaling = Scaling2D::Absolute;
        uint32_t Flags = 0;

        static constexpr std::array<TexturedVertex, 4> GetDefaultVertices()
        {
            auto texCoords = GetDefaultTexCoords();
            return {
                TexturedVertex({ -0.5f, 0.5f, 0.0f }, glm::vec4(1.0f), texCoords.at(0)),
                TexturedVertex({ -0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), texCoords.at(1)),
                TexturedVertex({ 0.5f, -0.5f, 0.0f }, glm::vec4(1.0f), texCoords.at(2)),
                TexturedVertex({ 0.5f, 0.5f, 0.0f }, glm::vec4(1.0f), texCoords.at(3)),
            };
        }

        static constexpr std::array<uint32_t, 6> GetDefaultIndices()
        {
            return { 0, 1, 2, 2, 3, 0 };
        }

        static constexpr std::array<glm::vec2, 4> GetDefaultTexCoords()
        {
            return {
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
            };
        }
    };
}