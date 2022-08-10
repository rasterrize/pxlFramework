#pragma once

#include <GLFW/glfw3.h>

namespace pxl
{
    enum class RendererAPI
    {
        OpenGL,
        Vulkan,
        DirectX12,
    };

    class Renderer
    {
    public:
        static void Init(RendererAPI api);
        static void Shutdown();
        
        static void Clear();
    private:
        static bool s_Enabled;
        static RendererAPI s_RendererAPI;
        static GLFWwindow* s_WindowHandle;
    };
}