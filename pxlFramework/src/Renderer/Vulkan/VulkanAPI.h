#pragma once

// #define VOLK_IMPLEMENTATION
#include <volk/volk.h>

#include "Renderer/GraphicsAPI.h"

namespace pxl
{
    class VulkanAPI : public GraphicsAPI
    {
    public:
        VulkanAPI();
        ~VulkanAPI();

        virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext() override;

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