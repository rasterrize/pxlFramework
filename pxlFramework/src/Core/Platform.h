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

        // Will return empty strings when cancelled
        static std::string OpenFile(const std::shared_ptr<Window> window, const char* filter);
        static std::string SaveFile(const std::shared_ptr<Window> window, const char* filter);
    };
}