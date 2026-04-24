#pragma once

#include <GLFW/glfw3.h>

namespace pxl
{
    /// @brief Universal platform utils
    namespace Platform
    {
        inline double GetTimeRunning()
        {
            return glfwGetTime();
        }

        inline std::string GetClipboardString()
        {
            return glfwGetClipboardString(nullptr);
        }

        inline void SetClipboardString(const std::string& string)
        {
            glfwSetClipboardString(nullptr, string.c_str());
        }

        inline bool IsRawInputSupported()
        {
            return glfwRawMouseMotionSupported();
        }
    };
}