#pragma once

#include <GLFW/glfw3.h>

#include "GraphicsContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

namespace pxl
{
    enum class RendererAPI
    {
        OpenGL,
        Vulkan,
        DirectX12
    };

    class Renderer
    {
    public:
        static void Init(RendererAPI api);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }

        static const RendererAPI GetRendererAPI() { return s_RendererAPI; }
        static const std::shared_ptr<Shader> GetShader() { return s_Shader; }
        
        static void Clear();
        static void SetClearColour(float r, float g, float b, float a);

        static void Draw(); // temp
    private:
        static bool s_Enabled;
        static RendererAPI s_RendererAPI;
        static GLFWwindow* s_WindowHandle; // Might need to change this with DX12
        static std::unique_ptr<GraphicsContext> s_GraphicsContext;
        
        static std::shared_ptr<VertexBuffer> s_VertexBuffer; // do these need to be shared pointers when they are static and always exist?
        static std::shared_ptr<VertexArray> s_VertexArray;
        static std::shared_ptr<IndexBuffer> s_IndexBuffer;
        static std::shared_ptr<Shader> s_Shader;
    };
}   