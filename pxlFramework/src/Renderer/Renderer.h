#pragma once

#include "Camera.h"
#include "Core/Events/EventHandler.h"
#include "Core/Events/WindowEvents.h"
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
#include "ShaderManager.h"
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
        float PreviousFrameTime = 0.0f;

        // Must be populated when returned
        FrameStats FrameStats = {};
        GraphicsContextStats GraphicsContextStats = {};
        GraphicsDeviceStats GraphicsDeviceStats = {};
    };

    namespace RendererConstants
    {
#ifdef PXL_DEBUG
        static const std::filesystem::path k_FrameworkShaderDirectory = PXL_SOURCE_RESOURCE_DIRECTORY "/shaders";
#else
        static const std::filesystem::path k_FrameworkShaderDirectory = "resources/shaders";
#endif
        static const uint32_t k_MaxFramesInFlight = 2;
    }

    class Renderer
    {
    public:
        Renderer(const RendererConfig& config);
        ~Renderer();

        RendererConfig GetConfig() const { return m_Config; }

        /// @brief Collects stats from the various renderer components, combines them, and
        /// returns them as one structure
        RendererStats GetStats() const;

        /// @brief Gets this renderer's GraphicsContext. It's not recommended you interact with this outside of the renderer class.
        /// @return The renderer's GraphicsContext.
        GraphicsContext& GetGraphicsContext() const { return *m_GraphicsContext; }

        /// @brief Gets this renderer's GraphicsDevice. It's not recommended you interact with this outside of the renderer class.
        /// @return The renderer's GraphicsDevice.
        GraphicsDevice& GetGraphicsDevice() const { return *m_GraphicsDevice; }

        void SetClearColour(const glm::vec4& colour);

        void SetVerticalSync(bool value);

        /// @brief Submits a quad primitive to the renderer to be drawn.
        void Submit(const Quad& quad);

        /// @brief Submits a line primitive to the renderer to be drawn.
        void Submit(const Line& line);

        /// @brief Submits a mesh to the renderer to be drawn using a position, rotation, and scale.
        void Submit(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        /// @brief Convenience function that returns the renderer's previous frame time as an FPS value.
        /// @return The frames-per-second calculated.
        float GetFramesPerSecond() const { return 1000.0f / m_RendererStats.PreviousFrameTime; }

        /// @brief Initializes ImGui. When this renderer is associated with an application, the application's OnGUIRender function will be called.
        void InitImGui();
        bool IsImGuiInitialized() const { return m_ImGuiRenderer != nullptr; }

        /// @brief Recreates all pipelines and reloads their shaders.
        void ReloadPipelines();

        /// @brief Clears the shader cache directory
        void ClearShaderCache() { m_ShaderManager->ClearCache(); }

        std::shared_ptr<Camera> GetCamera3D() const { return m_Camera3D; }

    private:
        friend class Application;
        void Begin();
        void End();

        void Flush();

        glm::mat4 CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        void ResetFrameStats() { memset(&m_FrameStats, 0, sizeof(FrameStats)); }

        void OnWindowFBResize(const WindowFBResizeEvent& e);

    private:
        RendererConfig m_Config;

        std::unique_ptr<GraphicsAPI> m_GraphicsAPI = nullptr;
        std::unique_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        std::unique_ptr<GraphicsDevice> m_GraphicsDevice = nullptr;

        std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer = nullptr;

        std::unique_ptr<ShaderManager> m_ShaderManager = nullptr;

        std::shared_ptr<EventHandler<WindowFBResizeEvent>> m_WindowFBResizeHandler = nullptr;

        std::shared_ptr<GPUBuffer> m_TriangleBuffer = nullptr;
        std::shared_ptr<GraphicsPipeline> m_TrianglePipeline = nullptr;

        std::shared_ptr<Texture> m_WhitePixelTexture = nullptr;
        std::shared_ptr<Texture> m_ErrorTexture = nullptr;
        struct PerFrameData
        {
            std::unique_ptr<VertexBatch<TexturedVertex>> QuadBatch;
            std::shared_ptr<GPUBuffer> UniformBuffer;
        };

        std::shared_ptr<GPUBuffer> m_QuadIndexBuffer = nullptr;
        std::shared_ptr<GraphicsPipeline> m_QuadPipeline = nullptr;


        FrameStats m_FrameStats = {};
        RendererStats m_RendererStats = {};
        std::vector<PerFrameData> m_PerFrameData;
        PerFrameData* m_CurrentFrameData;

        std::shared_ptr<Camera> m_Camera3D = nullptr;
        uint32_t m_FrameIndex = 0;
    };
}