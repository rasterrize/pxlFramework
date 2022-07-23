#include <GLFW/glfw3.h>

namespace pxl
{
    class Window
    {
    public:
        static void Init(unsigned int width, unsigned int height, std::string title);
        static void Update();
        static void Shutdown();
    private:
        void SetCallbacks();
    private:
        static GLFWwindow* s_Window;
    };
}