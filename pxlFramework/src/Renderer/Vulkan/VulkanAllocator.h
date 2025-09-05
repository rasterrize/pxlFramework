#pragma once
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <Volk/volk.h>
#include <vma/vk_mem_alloc.h>

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