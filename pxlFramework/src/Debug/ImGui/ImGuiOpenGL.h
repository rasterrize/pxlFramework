#include "../../Core/Window.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

namespace pxl
{
    class pxl_ImGui
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Update();
        static void Shutdown();
    private:
        static GLFWwindow* m_WindowHandle;
        static bool s_Enabled;
    };
}