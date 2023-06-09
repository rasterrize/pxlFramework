#include <GLFW/glfw3.h>

// platform specific utilities (getting time or date from operating system, etc)
namespace pxl
{ 
    class Platform
    {
    public:
        static double GetTime() { if (glfwInit()) { return glfwGetTime(); } else return 0.0f; } // should be changed to check operating system possibly
    private:
    };
}
