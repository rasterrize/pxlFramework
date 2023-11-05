#pragma once

#include "../Core/Window.h"
#include "../Core/Window/WindowGLFW.h"
#include "RendererAPIType.h"
#include "RendererAPI.h"
#include "RendererData.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

#include "VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    class Renderer
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }

        static const RendererAPIType GetAPIType() { return s_RendererAPIType; }

        static const float GetFPS() { return s_FPS; }
        static const float GetFrameTimeMS() { return 1 / s_FPS * 1000.0f; }

        static void Clear();
        static void SetClearColour(const glm::vec4& colour);

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& camera);

        static void Begin();
        static void End();

        static void Draw();

        static void AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour);
        //static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
        //static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& tint);
        //static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec2& textureIndex);

        static void AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour);
        //static void AddTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex);

        static void AddLine(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour);

        //static void AddMesh(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);


        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadVertexCount = 0;
            uint32_t QuadIndexCount = 0;
            uint32_t LineVertexCount = 0;
            
            uint32_t GetTriangleCount() { return QuadIndexCount / 3; }
            uint32_t GetEstimatedVRAMUsage() { return 
            (QuadVertexCount * (uint32_t)sizeof(TriangleVertex)) +
            (QuadIndexCount * 4) +
            (LineVertexCount * (uint32_t)sizeof(LineVertex)); // In Bytes
            
            }
        };

        static void ResetStats() { memset(&s_Stats, 0, sizeof(Statistics)); }
        static const Statistics& GetStats() { return s_Stats; }
    private:
        static void CalculateFPS();

        static void StartQuadBatch();
        static void StartCubeBatch();
        static void StartLineBatch();

        static void EndQuadBatch();
        static void EndCubeBatch();
        static void EndLineBatch();

        static void DrawQuads();
        static void DrawCubes();
        static void DrawLines();

    private:
        friend class Window;
        friend class WindowGLFW;

        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;
        static std::unique_ptr<RendererAPI> s_RendererAPI;

        static std::shared_ptr<VertexArray> s_QuadVAO;
        static std::shared_ptr<VertexArray> s_CubeVAO;
        static std::shared_ptr<VertexArray> s_LineVAO;
        static std::shared_ptr<VertexArray> s_MeshVAO;

        static float s_FPS;
        static uint32_t s_FrameCount;
        static float s_TimeAtLastFrame;

        static std::vector<std::shared_ptr<Mesh>> s_Meshes;

        static Statistics s_Stats;
    };
}   