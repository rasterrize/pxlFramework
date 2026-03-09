#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"

// platform specific utilities (getting time or date from operating system, etc)
namespace pxl
{
    class Platform
    {
    public:
        static double GetTime() { return glfwInit() ? glfwGetTime() : 0.0f; }

        static uint64_t GetTimerFrequency() { return glfwGetTimerFrequency(); }

        static std::string GetClipboardString() { return glfwGetClipboardString(NULL); }
        static void SetClipboardString(const std::string& string) { glfwSetClipboardString(NULL, string.c_str()); }

        // Will return empty strings when cancelled
        static std::string OpenFile(const std::shared_ptr<Window> window, const char* filter);
        static std::string SaveFile(const std::shared_ptr<Window> window, const char* filter);

        // WARNING: this function only works on windows 10 20h1 or greater
        static void UseImmersiveDarkMode(GLFWwindow* window);
    };
}