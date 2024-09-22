#pragma once

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanInstance
    {
    public:
        static void Init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

        static VkInstance Get() { return s_Instance; }

    private:
        static inline VkInstance s_Instance = VK_NULL_HANDLE;
    };
}