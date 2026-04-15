#pragma once

#include "Camera.h"
#include "Core/Events/EventHandler.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Window.h"
#include "Renderer/AnimatedTexture.h"
#include "Renderer/Camera.h"
#include "Renderer/GraphicsAPI.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/ImGuiRenderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Primitives/2D/Quad.h"
#include "Renderer/Primitives/2D/Sprite.h"
#include "Renderer/Primitives/3D/Line.h"
#include "Renderer/RendererConfig.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderManager.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexBatch.h"

namespace pxl
{
    namespace RendererConstants
    {
        static const std::filesystem::path k_FrameworkShaderDirectory = PXL_RESOURCE_DIRECTORY "/shaders";

        static const uint32_t k_VerticesPerQuad = 4;
        static const uint32_t k_IndicesPerQuad = 6;

        static const uint32_t k_MaxFramesInFlight = 2;
    }

    class Renderer
    {
    public:
        Renderer(const RendererConfig& config);
        ~Renderer();

        const RendererConfig& GetConfig() const { return m_Config; }

        /// @brief Gets this renderer's GraphicsContext. It's not recommended you interact with this outside of the renderer class.
        /// @return The renderer's GraphicsContext.
        GraphicsContext& GetGraphicsContext() const { return *m_GraphicsContext; }

        /// @brief Gets this renderer's GraphicsDevice. It's not recommended you interact with this outside of the renderer class.
        /// @return The renderer's GraphicsDevice.
        GraphicsDevice& GetGraphicsDevice() const { return *m_GraphicsDevice; }

        /// @brief Submits a quad primitive to the renderer to be drawn.
        void Submit(const Quad& quad);

        /// @brief Submits a line primitive to the renderer to be drawn.
        void Submit(const Line& line);

        /// @brief Submits a mesh to the renderer to be drawn using a position, rotation, and scale.
        void Submit(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        /// @brief Initializes ImGui. When this renderer is associated with an application, the application's OnGUIRender function will be called.
        void InitImGui();
        bool IsImGuiInitialized() const { return m_ImGuiRenderer != nullptr; }

        void SetClearColour(const glm::vec4& colour);

        void SetVerticalSync(bool value);

        void SetTripleBuffering(bool value);

        void SetAllowTearing(bool value);

        /// @brief Recreates all pipelines and reloads their shaders.
        void ReloadPipelines();

        /// @brief Clears the shader cache directory.
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
        RendererConfig m_Config = {};

        std::unique_ptr<GraphicsAPI> m_GraphicsAPI = nullptr;
        std::unique_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        std::unique_ptr<GraphicsDevice> m_GraphicsDevice = nullptr;

        std::unique_ptr<ShaderManager> m_ShaderManager = nullptr;

        std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer = nullptr;

        std::shared_ptr<EventHandler<WindowFBResizeEvent>> m_WindowFBResizeHandler = nullptr;

        std::shared_ptr<TextureHandler> m_TextureHandler;

        std::shared_ptr<Texture> m_WhitePixelTexture = nullptr;
        std::shared_ptr<Texture> m_ErrorTexture = nullptr;
        struct PerFrameData
        {
            std::unique_ptr<VertexBatch<TexturedVertex>> QuadBatch;
            std::shared_ptr<GPUBuffer> UniformBuffer;
        };

        std::shared_ptr<GPUBuffer> m_QuadIndexBuffer = nullptr;
        std::shared_ptr<GraphicsPipeline> m_QuadPipeline = nullptr;

        std::shared_ptr<Camera> m_Camera3D = nullptr;

        std::vector<PerFrameData> m_PerFrameData;
        PerFrameData* m_CurrentFrameData;

        uint32_t m_FrameIndex = 0;
    };
}