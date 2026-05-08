#pragma once

#include <volk/volk.h>

#include "Renderer/GraphicsInstance.h"

namespace pxl
{
    class VulkanGraphicsInstance : public GraphicsInstance
    {
    public:
        VulkanGraphicsInstance();
        ~VulkanGraphicsInstance();

        virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext(const GraphicsContextSpecs& specs) override;

        virtual std::unique_ptr<GraphicsDevice> CreateGraphicsDevice(const GraphicsDeviceSpecs& specs) override;

    private:
        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT types,
            const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
            void* userData);

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    };
}