#include "KeyCodes.h"
#include <GLFW/glfw3.h>

namespace pxl
{
    class Input
    {
    public:
        static void Init();
        static void Shutdown();
    
        static bool IsKeyPressed(KeyCode keycode);
        static bool IsKeyHeld(KeyCode keycode);

    private:
        static bool s_Enabled;
        static GLFWwindow* s_WindowHandle;
    };
}