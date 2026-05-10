#include "Monitor.h"

namespace pxl
{
    void Monitors::Init()
    {
        glfwSetMonitorCallback([](GLFWmonitor* monitor, int event)
        {
            if (event == GLFW_CONNECTED)
            {
                PXL_LOG_INFO("Monitor '{}' was connected", glfwGetMonitorName(monitor));
            }
            else if (event == GLFW_DISCONNECTED)
            {
                PXL_LOG_INFO("Monitor '{}' was disconnected", glfwGetMonitorName(monitor));
            }

            Process();
        });

        Process();
    }

    void Monitors::Shutdown()
    {
        s_Monitors.clear();
        glfwSetMonitorCallback(nullptr);
    }

    const Monitor& Monitors::GetPrimary()
    {
        // First monitor is always the primary monitor
        return s_Monitors.front();
    }

    const Monitor& Monitors::Get(uint32_t index)
    {
        if (index >= s_Monitors.size())
            return s_Monitors.front();

        return s_Monitors.at(index);
    }

    void Monitors::Process()
    {
        PXL_PROFILE_SCOPE;

        s_Monitors.clear();

        int monitorCount;
        auto glfwMonitors = glfwGetMonitors(&monitorCount);

        for (uint8_t i = 0; i < monitorCount; i++)
        {
            Monitor monitor;
            monitor.GLFWMonitor = glfwMonitors[i];
            monitor.Index = i + 1;
            monitor.Name = glfwGetMonitorName(glfwMonitors[i]);
            glfwGetMonitorPos(glfwMonitors[i], &monitor.Position.x, &monitor.Position.y);

            int32_t widthMM, heightMM;
            glfwGetMonitorPhysicalSize(glfwMonitors[i], &widthMM, &heightMM);
            monitor.PhysicalSize = Size2D(static_cast<uint32_t>(widthMM), static_cast<uint32_t>(heightMM));

            int32_t vidModeCount = 0;
            auto vidModes = glfwGetVideoModes(glfwMonitors[i], &vidModeCount);

            for (int32_t v = 0; v < vidModeCount; v++)
                monitor.VideoModes.push_back(&vidModes[v]);

            monitor.IsPrimary = monitor.GLFWMonitor == glfwGetPrimaryMonitor();

            s_Monitors.push_back(monitor);
        }
    }
}