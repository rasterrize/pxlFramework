#pragma once

#include "Camera.h"
#include "Core/Colour.h"
#include "Core/Window.h"
#include "GraphicsContext.h"
#include "Pipeline.h"
#include "Primitives/Cube.h"
#include "Primitives/Line.h"
#include "Primitives/Quad.h"
#include "RendererAPI.h"
#include "RendererAPIType.h"
#include "RendererData.h"
#include "Shader.h"
#include "Texture.h"

namespace pxl
{
    enum class RendererGeometryTarget
    {
        Quad,
        Cube,
        Line,
        Mesh,
    };

    enum class FramerateCap
    {
        Unlimited,
        Custom,
        GSYNC,
    };

    class Renderer
    {
    public:
        static void Init(const std::shared_ptr<Window>& window);
        static void Shutdown();

        static bool IsInitialized() { return s_Enabled; }

        static RendererAPIType GetCurrentAPI() { return s_RendererAPIType; }
        static std::shared_ptr<GraphicsContext> GetGraphicsContext() { return s_ContextHandle; }

        static void SetClearColour(const glm::vec4& colour);
        static void SetClearColour(ColourName colour);

        static void ResizeViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }
        static void ResizeScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetScissor(x, y, width, height); }

        // Set the camera for the given GeometryTarget
        static void SetCamera(RendererGeometryTarget target, const std::shared_ptr<Camera>& camera);

        // Set the camera for every GeometryTarget
        static void SetCameraAll(const std::shared_ptr<Camera>& camera);

        static std::shared_ptr<GraphicsPipeline> GetPipeline(RendererGeometryTarget target);

        // Set a custom pipeline for the given GeometryTarget
        static void SetPipeline(RendererGeometryTarget target, const std::shared_ptr<GraphicsPipeline>& pipeline);

        // (NOT RECOMMENDED) Set a custom pipeline for every GeometryTarget
        static void SetPipelineAll(const std::shared_ptr<GraphicsPipeline>& pipeline);

        static void AddQuad(const Quad& quad);
        static void AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour);

        static void AddCube(const Cube& cube);
        static void AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour);

        static void AddLine(const Line& line);
        static void AddLine(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec3& rotation, const glm::vec4& colour);

        static void DrawMesh(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        // Reset the static geometry data of the give GeometryTarget
        static void ResetStaticGeometry(RendererGeometryTarget target);

        // Uploads the static geometry to the GPU
        static void StaticGeometryReady();

        static void AddStaticQuad(const Quad& quad);
        static void AddStaticQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour);
        static void AddStaticCube(const Cube& cube);
        static void AddStaticCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour);

        struct Statistics
        { // clang-format off
            float FrameTime          = 0.0f;
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
            uint32_t MeshCount       = 0;
            uint32_t MeshVertexCount = 0;
            uint32_t MeshIndexCount  = 0;
            uint32_t TextureBinds    = 0;
            uint32_t PipelineBinds   = 0;

            uint32_t GetTotalTriangleCount() { return (QuadIndexCount / 3) + (CubeIndexCount / 3) + (MeshIndexCount / 3); }
            uint32_t GetTotalVertexCount() { return QuadVertexCount + CubeVertexCount + LineVertexCount + MeshVertexCount; }
            uint32_t GetTotalIndexCount() { return QuadIndexCount + CubeIndexCount + MeshIndexCount; }
        }; // clang-format on

        // Gets the statistics of the current frame
        static const Statistics& GetStats() { return s_Stats; }

        static float GetFPS() { return s_Stats.FPS; }
        static float GetFrameTimeMS() { return s_Stats.FrameTime; }

    private:
        friend class Application;
        static void CalculateFPS();
        static void Begin();
        static void End();

        static void Flush();

        static float GetTextureIndex(const std::shared_ptr<Texture>& texture);

        static glm::mat4 CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

        static void ResetStats()
        {
            memset(&s_Stats, 0, sizeof(Statistics));
        }

    private:
        static inline bool s_Enabled = false;

        static inline RendererAPIType s_RendererAPIType = RendererAPIType::None;

        static inline std::unique_ptr<RendererAPI> s_RendererAPI = nullptr;

        static inline std::shared_ptr<GraphicsContext> s_ContextHandle = nullptr;

        static inline std::shared_ptr<Camera> s_QuadCamera = nullptr;
        static inline std::shared_ptr<Camera> s_CubeCamera = nullptr;
        static inline std::shared_ptr<Camera> s_LineCamera = nullptr;

        static inline uint32_t s_FrameCount = 0;
        static inline double s_TimeAtLastFrame = 0.0f;

        static inline Statistics s_Stats = {};
        static inline RendererLimits s_Limits = {};
    };
}