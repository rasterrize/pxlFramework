#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "VulkanDevice.h"

namespace pxl
{
    class VulkanAllocator
    {
    public:
        static void Init(VkInstance instance, const std::shared_ptr<VulkanDevice>& device);
        static VmaAllocator Get() { return s_Allocator; }
        static void Shutdown();
    private:
        static inline VmaAllocator s_Allocator = nullptr;
    };
}