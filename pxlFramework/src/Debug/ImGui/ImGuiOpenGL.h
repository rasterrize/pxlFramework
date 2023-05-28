#include <imgui.h>
#include <GLFW/glfw3.h>

namespace pxl
{
    class pxl_ImGui
    {
    public:
        static void Init(GLFWwindow* window);
        static void Update();
        static void Shutdown();
    private:
        static bool s_Enabled;
    };
}