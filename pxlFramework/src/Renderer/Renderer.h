#pragma once

#include "../Core/Window.h"
#include "RendererAPIType.h"
#include "Shader.h"
#include "VertexArray.h"
#include "../Core/Math.h"
#include "RendererAPI.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    struct Vertex
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec2 TexCoords = glm::vec2(0.0f);
    };

    struct Mesh
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;

        glm::mat4 Transform = glm::mat4(0.0f);

        void Translate(float x, float y, float z)
        {
            Transform = glm::translate(glm::mat4(1.0f), { x, y, z });
        }
    };

    class Renderer
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }

        static const RendererAPIType GetAPIType() { return s_RendererAPIType; }
        static std::shared_ptr<Shader> GetShader() { return s_RendererAPI->GetShader(); }

        static const float GetFPS() { return s_FPS; }
        static const float GetFrameTimeMS() { return 1 / s_FPS * 1000.0f; }

        static void Clear();
        static void SetClearColour(const vec4& colour);

        static void DrawArrays(int count);
        static void DrawLines(int count);
        static void DrawIndexed();
        
        static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
        static void Submit(const std::shared_ptr<Shader>& shader);
        static void Submit(const std::shared_ptr<Mesh> mesh); // not a reference currently so things dont break

        //static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
        static void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
        static void DrawTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex);

        static void StartBatch();
        static void EndBatch();

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t VertexCount = 0;
            uint32_t IndiceCount = 0; // should be index
            
            uint32_t GetTriangleCount() { return IndiceCount / 3; }
        };

        static void ResetStats() { memset(&s_Stats, 0, sizeof(Statistics)); }
        static const Statistics& GetStats() { return s_Stats; }
    private:
        static void CalculateFPS();

    private:
        friend class WindowGLFW;

        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;

        static std::unique_ptr<RendererAPI> s_RendererAPI;

        //static std::unique_ptr<VertexBuffer> s_CubeVBO;

        static float s_FPS;
        static uint32_t s_FrameCount;
        static float s_TimeAtLastFrame;

        // Quad Buffer Data
        // static const size_t s_MaxQuadCount = 1000;
        // static const size_t s_MaxQuadVertexCount = s_MaxQuadCount * 4;
        // static const size_t s_MaxQuadIndexCount = s_MaxQuadCount * 6;

        // static std::array<Vertex, s_MaxQuadVertexCount> s_QuadVertices;
        // static std::array<uint32_t, s_MaxQuadIndexCount> s_QuadIndices;

        // static size_t Renderer::s_QuadVertexCount;
        // static size_t Renderer::s_QuadIndexCount;

        // Cube Buffer Data
        // static const size_t s_MaxCubeCount = 1000;
        // static const size_t s_MaxCubeVertexCount = s_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved. This is technically just a bunch of quads
        // static const size_t s_MaxCubeIndexCount = s_MaxCubeCount * 36;

        // static std::array<Vertex, s_MaxCubeVertexCount> s_CubeVertices; 
        // static std::array<uint32_t, s_MaxCubeIndexCount> s_CubeIndices;

        static std::vector<std::shared_ptr<Mesh>> s_Meshes;

        static Statistics s_Stats;
    };
}   