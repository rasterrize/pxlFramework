#pragma once

#include "Core/Events/WindowEvents.h"
#include "Core/Rect.h"
#include "Core/Window.h"
#include "Platform/SleepTimer.h"
#include "Renderer/AnimatedTexture.h"
#include "Renderer/Camera.h"
#include "Renderer/GraphicsAPI.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/ImGuiRenderer.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Primitives/2D/Quad.h"
#include "Renderer/Primitives/3D/Line.h"
#include "Renderer/RendererConfig.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderManager.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexBatch.h"

namespace pxl
{
    template<typename T>
    class EventHandler;

    class Renderer
    {
    public:
        Renderer(const RendererConfig& config);
        ~Renderer();

        /// @brief Submits a quad primitive to the renderer to be drawn.
        void Submit(Quad& quad);

        /// @brief Submits a quad primitive to the renderer to be drawn with a texture.
        void Submit(Quad& quad, const std::shared_ptr<Texture>& texture, const std::array<glm::vec2, 4>& texCoords = Quad::GetDefaultTexCoords());

        /// @brief Submits a quad primitive to the renderer to be drawn with a subtexture.
        void Submit(Quad& quad, const SubTexture& subTexture);

        /// @brief Submits a quad primitive to the renderer to be drawn with an animated texture.
        void Submit(Quad& quad, AnimatedTexture& animatedTexture);

        /// @brief Initializes ImGui. When this renderer is associated with an application, the application's OnGUIRender function will be called.
        void InitImGui();
        bool IsImGuiInitialized() const { return m_ImGuiRenderer != nullptr; }

        void SetClearColour(const glm::vec4& colour);
        void SetVerticalSync(bool value);
        void SetTripleBuffering(bool value);
        void SetAllowTearing(bool value);
        void SetFramerateMode(FramerateMode mode);
        void SetCustomFramerateLimit(uint32_t limit) { m_Config.CustomFramerateLimit = limit; }
        void SetUnfocusedFramerateLimit(uint32_t limit) { m_Config.UnfocusedFramerateLimit = limit; }

        uint32_t GetAdaptiveSyncFramerateLimit() const { return m_AdaptiveSyncFramerateLimit; }

        /// @brief Recreates all pipelines and reloads their shaders.
        void ReloadPipelines();

        /// @brief Clears the shader cache directory.
        void ClearShaderCache() { m_ShaderManager->ClearCache(); }

        std::shared_ptr<Texture> CreateTexture(TextureSpecs& specs);

        std::shared_ptr<OrthographicCamera> GetCamera2D() const { return m_CameraUI; }

        void Suspend() { m_Suspend = true; }
        void Unsuspend() { m_Suspend = false; }

        bool IsSuspended() const { return m_Suspend; }

        const RendererConfig& GetConfig() const { return m_Config; }

        /// @brief Gets this renderer's GraphicsContext. It's not recommended you interact with this outside of the renderer class.
        GraphicsContext& GetGraphicsContext() const { return *m_GraphicsContext; }

        /// @brief Gets this renderer's GraphicsDevice. It's not recommended you interact with this outside of the renderer class.
        GraphicsDevice& GetGraphicsDevice() const { return *m_GraphicsDevice; }

        struct FrameStatistics
        {
            uint64_t FrameCountIndex;
            uint32_t FrameInFlightIndex;
            double FrameTime;
            double RenderTime;
            double GraphicsDeviceWaitTime;
            double FramerateLimitWaitTime;
            double FramerateLimitSleepTime;
            double FramerateLimitSpinTime;

            float GetFPS() const { return 1000.0f / static_cast<float>(FrameTime); }
        };

        const FrameStatistics& GetFrameStats() const { return m_FrameStats; }

    private:
        friend class Application;
        void Begin();
        void End();
        void LimitFramerateIfNecessary();

        void Flush();

        glm::mat4 CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        // TODO: general function, move into utils or something
        glm::vec2 PositionOfAnchorOnRect(const RectF& rect, Anchor2D anchor);
        glm::vec2 OffsetOfOriginOnQuad(const Quad& quad);

        Quad Process(const Quad& quad);

        void OnWindowFBResize(const WindowFBResizeEvent& e);

    private:
        RendererConfig m_Config = {};

        std::unique_ptr<GraphicsAPI> m_GraphicsAPI;
        std::unique_ptr<GraphicsContext> m_GraphicsContext;
        std::unique_ptr<GraphicsDevice> m_GraphicsDevice;

        std::unique_ptr<ShaderManager> m_ShaderManager;
        std::shared_ptr<TextureHandler> m_TextureHandler;
        std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

        std::shared_ptr<EventHandler<WindowFBResizeEvent>> m_WindowFBResizeHandler;

        std::shared_ptr<Texture> m_WhitePixelTexture;

        struct PerFrameData
        {
            std::unique_ptr<VertexBatch<TexturedVertex>> QuadBatch;
            std::shared_ptr<GPUBuffer> UniformBuffer;
        };

        std::shared_ptr<GPUBuffer> m_QuadIndexBuffer;
        std::shared_ptr<GraphicsPipeline> m_QuadPipeline;

        std::shared_ptr<OrthographicCamera> m_CameraUI;

        std::vector<PerFrameData> m_PerFrameData;
        PerFrameData* m_CurrentFrameData = nullptr;

        uint32_t m_FrameInFlightIndex = 0;
        uint64_t m_FrameCount = 0;

        bool m_Suspend = false;

        uint32_t m_AdaptiveSyncFramerateLimit = 0;

        std::unique_ptr<SleepTimer> m_SleepTimer;

        FrameStatistics m_FrameStats = {};

        std::chrono::steady_clock::time_point m_BeginPoint = {};
    };
}