#pragma once

#include <GLFW/glfw3.h>

namespace pxl
{
    struct VideoMode
    {
        VideoMode(const GLFWvidmode* glfwMode)
            : Size(glfwMode->width, glfwMode->height),
              RefreshRate(glfwMode->refreshRate),
              BitDepth(glfwMode->redBits, glfwMode->greenBits, glfwMode->blueBits),
              GLFWVidMode(glfwMode)
        {
        }

        std::string ToString() { return std::format("{}x{}, {}hz", Size.Width, Size.Height, RefreshRate); }

        Size2D Size = {};
        uint32_t RefreshRate = 0;
        glm::ivec3 BitDepth = {};
        const GLFWvidmode* GLFWVidMode = nullptr;
    };

    struct Monitor
    {
        uint8_t Index = 1; // refers to the operating system's ID for the monitor
        std::string Name = "Unnamed Monitor";
        glm::ivec2 Position = {};
        std::vector<VideoMode> VideoModes;
        Size2D PhysicalSize = {};
        bool IsPrimary = false;
        GLFWmonitor* GLFWMonitor = nullptr;

        VideoMode GetCurrentVideoMode() const { return VideoMode(glfwGetVideoMode(GLFWMonitor)); }
    };

    class Monitors
    {
    public:
        static void Init();

        static void Shutdown();

        static const Monitor& GetPrimary();

        static const Monitor& Get(uint32_t index);

        static const std::vector<Monitor>& GetAll() { return s_Monitors; }

    private:
        static void Process();

        inline static std::vector<Monitor> s_Monitors;
    };
}