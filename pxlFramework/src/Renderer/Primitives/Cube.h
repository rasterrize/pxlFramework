#pragma once

#include <glm/glm.hpp>

#include "Primitive.h"

#include "Renderer/Vertices.h"

namespace pxl
{
    struct Cube
    {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Size;
        glm::vec4 Colour;

        bool Contains(const glm::vec3& position)
        {            
            PXL_PROFILE_SCOPE;
            
            auto left = Position.x - Size.x / 2.0f;
            auto right = Position.x + Size.x / 2.0f;
            auto bottom = Position.y - Size.y / 2.0f;
            auto top = Position.y + Size.y / 2.0f;
            auto front = Position.z + Size.z / 2.0f; 
            auto back = Position.z - Size.z / 2.0f;

            if (position.x >= left && position.x <= right && position.y >= bottom && position.y <= top && position.z >= front && position.z <= back)
                return true;
            
            return false;
        }

        static constexpr std::array<CubeVertex, 24> GetDefaultVertices()
        {
            return {
                CubeVertex({ -0.5f,  0.5f, 0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5f, -0.5f, 0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5f, -0.5f, 0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5f,  0.5f, 0.5f }, glm::vec4(1.0f)),

                CubeVertex({  0.5f, 0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5, -0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5, -0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5,  0.5f, -0.5f }, glm::vec4(1.0f)),

                CubeVertex({ -0.5,  0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5, -0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5, -0.5f,  0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5,  0.5f,  0.5f }, glm::vec4(1.0f)),

                CubeVertex({ -0.5, 0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5, 0.5f,  0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5, 0.5f,  0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5, 0.5f, -0.5f }, glm::vec4(1.0f)),

                CubeVertex({ 0.5,  0.5f,  0.5f }, glm::vec4(1.0f)),
                CubeVertex({ 0.5, -0.5f,  0.5f }, glm::vec4(1.0f)),
                CubeVertex({ 0.5, -0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({ 0.5,  0.5f, -0.5f }, glm::vec4(1.0f)),

                CubeVertex({ -0.5, -0.5f,  0.5f }, glm::vec4(1.0f)),
                CubeVertex({ -0.5, -0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5, -0.5f, -0.5f }, glm::vec4(1.0f)),
                CubeVertex({  0.5, -0.5f,  0.5f }, glm::vec4(1.0f)),
            };
        }

        static constexpr std::array<uint32_t, 6> GetDefaultIndices()
        {
            return { 0, 1, 2, 2, 3, 0 };
        }
    };
}