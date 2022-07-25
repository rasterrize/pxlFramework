#include <GLFW/glfw3.h>

namespace pxl
{
    class Window
    {
    public:
        static void Init(unsigned int width, unsigned int height, std::string title);
        static void Shutdown();
    private:
        static void Update();
        friend class Application;
        static void SetCallbacks();
    private:
        static GLFWwindow* s_Window;
    };
}