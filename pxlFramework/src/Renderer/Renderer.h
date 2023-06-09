#pragma once

#include <GLFW/glfw3.h>

#include "../Core/Math.h"

#include "GraphicsContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"

#include "RendererAPI.h"

namespace pxl
{
    enum class RendererAPIType
    {
        None,
        OpenGL,
        Vulkan,
        DirectX11,
        DirectX12
    };

    struct Mesh
    {
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
    };

    class Renderer
    {
    public:
        static void Init(RendererAPIType api);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }

        static const RendererAPIType GetRendererAPIType() { return s_RendererAPIType; }
        static std::shared_ptr<Shader> GetShader() { return s_RendererAPI->GetShader(); }

        static const float GetFPS() { return s_FPS; }
        static const float GetFrameTimeMS() { return 1 / s_FPS * 1000.0f; }

        static void Clear();
        static void SetClearColour(const vec4& colour);

        static void DrawArrays(int count);
        static void DrawLines(int count);
        static void DrawIndexed();
        
        static void Submit(const std::shared_ptr<Mesh>& mesh, float x, float y, float z);
        static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
        static void Submit(const std::shared_ptr<Shader>& shader);
    private:
        static void CalculateFPS();
    private:
        friend class Window;

        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;
        static GLFWwindow* s_WindowHandle; // Might need to change this with DX12

        static std::unique_ptr<RendererAPI> s_RendererAPI;

        static float s_FPS;
        static uint16_t s_FrameCount;
        static float s_TimeAtLastFrame;
        static float s_LastFPS; 

        //static std::vector<Mesh> s_Meshes;
    };
}   