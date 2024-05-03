#pragma once

//#include "../Core/Window.h"
#include "RendererAPIType.h"
#include "RendererAPI.h"
#include "RendererData.h"
//#include "Texture.h"
#include "Camera.h"
#include "GraphicsContext.h"

#include "VertexArray.h"
#include "Device.h"
#include "Pipeline.h"
//#include "Vulkan/VulkanRenderPass.h"

//#include <glm/matrix.hpp>
//class RendererAPI;
// class RendererData;
// class Shader;
//class Shader;
//class Camera;
// class VertexArray;
// class Device;
// class Pipeline;

namespace pxl
{
    class Window;
    enum class RendererAPIType;
    class Shader;

    class Renderer
    {
    public:
        static void Init(const std::shared_ptr<Window>& window);
        static void Shutdown();

        static bool IsInitialized() { return s_Enabled; }

        static RendererAPIType GetCurrentAPI() { return s_RendererAPIType; }
        static std::shared_ptr<GraphicsContext> GetGraphicsContext() { return s_ContextHandle; }

        static float GetFPS() { return s_FPS; }
        static float GetFrameTimeMS() { return 1 / s_FPS * 1000.0f; }

        static void Clear();
        static void SetClearColour(const glm::vec4& colour);

        static void ResizeViewport(uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(0, 0, width, height); }
        static void ResizeScissor(uint32_t width, uint32_t height) { s_RendererAPI->SetScissor(0, 0, width, height); }

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& camera);
        static void Submit(const std::shared_ptr<GraphicsPipeline>& pipeline);

        static void Begin();
        static void End();

        static void AddStaticQuad(const glm::vec3& position = glm::vec3(0.0f));
        static void StaticGeometryReady();
        static void DrawStaticQuads();
        
        static void AddQuad(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const glm::vec4& colour = glm::vec4(1.0f));
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
        };

        static void ResetStats() { memset(&s_Stats, 0, sizeof(Statistics)); }
        static const Statistics& GetStats() { return s_Stats; }
    private:
        friend class Window;
        static void CalculateFPS();

        static void Flush();

    private:
        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;
        static std::unique_ptr<RendererAPI> s_RendererAPI;
        static std::shared_ptr<GraphicsContext> s_ContextHandle;

        // test
        static std::shared_ptr<Buffer> s_StaticQuadVBO;
        static std::shared_ptr<VertexArray> s_StaticQuadVAO;
        static uint32_t s_StaticQuadCount;
        //static uint32_t s_StaticQuadVertexCount;

        static std::shared_ptr<VertexArray> s_QuadVAO;
        static std::shared_ptr<Buffer> s_QuadVBO;
        static std::shared_ptr<Buffer> s_QuadIBO;

        // For OpenGL
        static std::shared_ptr<VertexArray> s_CubeVAO;
        static std::shared_ptr<VertexArray> s_LineVAO;
        static std::shared_ptr<VertexArray> s_MeshVAO;

        static float s_FPS; // shouldn't store this
        static uint32_t s_FrameCount;
        static float s_TimeAtLastFrame;

        static std::vector<std::shared_ptr<Mesh>> s_Meshes;

        static Statistics s_Stats;
    };
}   