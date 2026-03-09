#pragma once

#include "Camera.h"
#include "Core/Window.h"
#include "GraphicsAPI.h"
#include "GraphicsContext.h"
#include "GraphicsPipeline.h"
#include "ImGuiRenderer.h"
#include "Mesh.h"
#include "Primitives/Line.h"
#include "Primitives/Quad.h"
#include "RendererConfig.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBatch.h"

namespace pxl
{
    struct FrameStats
    {
        uint32_t DrawCalls;
        uint32_t VertexCount;
        uint32_t IndexCount;
        uint32_t TextureBinds;
        uint32_t PipelineBinds;

        uint32_t GetTriangleCount() { return IndexCount / 3; }
    };

    struct RendererStats
    {
        uint64_t FrameCount = 0;
        float LastFrameTime = 0.0f;
        double TimeAtLastFrame = 0.0;
    };

    class Renderer
    {
    public:
        Renderer(const RendererConfig& config);
        ~Renderer();

        RendererConfig GetConfig() const { return m_Config; }

        const std::unique_ptr<GraphicsContext>& GetGraphicsContext() const { return m_GraphicsContext; }
        const std::unique_ptr<GraphicsDevice>& GetGraphicsDevice() const { return m_GraphicsDevice; }

        void SetClearColour(const glm::vec4& colour) { m_GraphicsContext->SetClearColour(colour); }

        void Submit(const Quad& quad);
        void Submit(const Line& line);
        void Submit(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        FrameStats& GetFrameStats() { return m_FrameStats; }
        RendererStats& GetRendererStats() { return m_RendererStats; }

        float GetFramesPerSecond() const { return 1000.0f / m_RendererStats.LastFrameTime; }

        void InitImGui();
        bool IsImGuiInitialized() const { return m_ImGuiRenderer != nullptr; }

    private:
        friend class Application;
        void Begin();
        void End();

        void Flush();

        glm::mat4 CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        void ResetFrameStats() { memset(&m_FrameStats, 0, sizeof(FrameStats)); }

    private:
        RendererConfig m_Config;

        std::unique_ptr<GraphicsAPI> m_GraphicsAPI = nullptr;
        std::unique_ptr<GraphicsDevice> m_GraphicsDevice = nullptr;
        std::unique_ptr<GraphicsContext> m_GraphicsContext = nullptr;

        std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer = nullptr;

        std::shared_ptr<GPUBuffer> m_TriangleBuffer;
        std::shared_ptr<GraphicsPipeline> m_TrianglePipeline;

        std::shared_ptr<Texture> m_WhitePixelTexture = nullptr;
        std::shared_ptr<Texture> m_ErrorTexture = nullptr;

        // std::unique_ptr<TextureHandler> m_TextureHandler = nullptr;

        std::unique_ptr<VertexBatch<TexturedVertex>> m_QuadBatch;
        std::shared_ptr<GPUBuffer> m_QuadIndexBuffer;
        std::shared_ptr<GraphicsPipeline> m_QuadPipeline;

        FrameStats m_FrameStats = {};
        RendererStats m_RendererStats = {};

        // std::shared_ptr<Camera> m_Camera = nullptr;
    };
}