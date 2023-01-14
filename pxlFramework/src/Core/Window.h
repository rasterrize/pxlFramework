#include <GLFW/glfw3.h>
#include "../Renderer/Renderer.h"

namespace pxl
{
    enum class WindowMode
    {
        Windowed,
        Borderless,
        Fullscreen
    };

    class Window
    {
    public:
        static void Init(unsigned int width, unsigned int height, std::string title, RendererAPI rendererAPI);
        static void Shutdown();

        //static const bool IsInitialized() { return s_Enabled; }

        //static void SetWindowSize();
        //static void SetWindowMode();

        static GLFWwindow* GetNativeWindow() { return s_Window; }
    private:
        friend class Application;
        static void Update();

        static void InitGLFWWindow(unsigned int width, unsigned int height, std::string title);

        static void SetCallbacks();
        static void WindowCloseCallback(GLFWwindow* window);
    private:
        static GLFWwindow* s_Window; // Might need seperate custom window object for DX12
    };
}