#include <GLFW/glfw3.h>

namespace pxl
{
    class Window
    {
    public:
        static void Init(unsigned int width, unsigned int height, std::string title);
    private:
        static void Update();
        static void Shutdown();
    private:
        static GLFWwindow* m_Window;
    };
}