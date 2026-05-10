#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include <versionhelpers.h>

namespace pxl::Platform::Windows
{
    static void EnableDarkModeIfSupported(GLFWwindow* window)
    {
        // FIXME: doesn't actually check if dark mode is supported, but this feature has been supported for a long time, so it's a low priority
        auto hwnd = glfwGetWin32Window(window);
        int useImmersiveDarkMode = 1;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useImmersiveDarkMode, sizeof(int));
    }
}