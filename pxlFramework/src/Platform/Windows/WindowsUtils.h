#pragma once
#ifdef _WIN64

    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>
    #include <dwmapi.h>
    #include <versionhelpers.h>

namespace pxl::Platform::Windows
{
    static void EnableDarkModeIfSupported(GLFWwindow* window)
    {
        auto hwnd = glfwGetWin32Window(window);
        int useImmersiveDarkMode = 1;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useImmersiveDarkMode, sizeof(int));
    }
}
#endif