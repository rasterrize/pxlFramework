#pragma once

#include "../Core/Window.h"
#include "RendererAPIType.h"
#include "RendererAPI.h"
#include "RendererData.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicsContext.h"
#include "Pipeline.h"
#include "Shader.h"

namespace pxl
{
    class Renderer
    {
    public:
        static void Init(const std::shared_ptr<Window>& window);
        static void Shutdown();

        static bool IsInitialized() { return s_Enabled; }

        static RendererAPIType GetCurrentAPI() { return s_RendererAPIType; }
        static std::shared_ptr<GraphicsContext> GetGraphicsContext() { return s_ContextHandle; }

        static void Clear();
        static void SetClearColour(const glm::vec4& colour);

        static void ResizeViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }
        static void ResizeScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetScissor(x, y, width, height); }

        // static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& camera);
        // static void Submit(const std::shared_ptr<GraphicsPipeline>& pipeline);

        static void Begin();
        static void End();

        static void AddStaticQuad(const glm::vec3& position = glm::vec3(0.0f));
        static void StaticGeometryReady();
        static void DrawStaticQuads();
        static void SetQuadsCamera(const std::shared_ptr<Camera>& camera) { s_QuadsCamera = camera; }
        
        static void AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour);
        static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::shared_ptr<Texture>& texture);
        //static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::shared_ptr<Texture2D>& texture, const glm::vec2& textureUV);
        //static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& tint);
        //static void AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec2& textureIndex);
        static void AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour);
        //static void AddTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex);
        static void AddLine(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour);
        //static void DrawMesh(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        struct Statistics
        {
            float FPS                = 0.0f;
            uint32_t DrawCalls       = 0;
            uint32_t QuadCount       = 0;
            uint32_t QuadVertexCount = 0;
            uint32_t QuadIndexCount  = 0;
            uint32_t CubeCount       = 0;
            uint32_t CubeVertexCount = 0;
            uint32_t CubeIndexCount  = 0;
            uint32_t LineCount       = 0;
            uint32_t LineVertexCount = 0;
            
            uint32_t GetTotalTriangleCount() { return (QuadIndexCount / 3) + (CubeIndexCount / 3); }
            uint32_t GetTotalVertexCount() { return QuadVertexCount + CubeVertexCount + LineVertexCount; }
            uint32_t GetTotalIndexCount() { return QuadIndexCount + CubeIndexCount; }
        };

        static void ResetStats() { memset(&s_Stats, 0, sizeof(Statistics)); }
        static const Statistics& GetStats() { return s_Stats; }

        static float GetFPS() { return s_Stats.FPS; }
        static float GetFrameTimeMS() { return 1 / s_Stats.FPS * 1000.0f; }
    private:
        friend class Application;
        static void CalculateFPS();

        static void Flush();
    private:
        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;
        static std::unique_ptr<RendererAPI> s_RendererAPI;
        static std::shared_ptr<GraphicsContext> s_ContextHandle;

        static std::shared_ptr<Camera> s_QuadsCamera;

        static uint32_t s_FrameCount;
        static float s_TimeAtLastFrame;

        static Statistics s_Stats;
    };
}   