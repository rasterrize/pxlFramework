#pragma once

#include <GLFW/glfw3.h>

// platform specific utilities (getting time or date from operating system, etc)
namespace pxl
{ 
    class Platform
    {
    public:
        static double GetTime() { return glfwInit() ? glfwGetTime() : 0.0f; }
    };
}