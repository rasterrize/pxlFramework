#include "VulkanAllocator.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VmaAllocator VulkanAllocator::s_Allocator = nullptr;

    void VulkanAllocator::Init(VkInstance instance, const std::shared_ptr<VulkanDevice>& device)
    {
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.physicalDevice = static_cast<VkPhysicalDevice>(device->GetPhysicalDevice());
        allocatorCreateInfo.device = static_cast<VkDevice>(device->GetDevice());
        allocatorCreateInfo.instance = instance;
        allocatorCreateInfo.pVulkanFunctions = nullptr;

        VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &s_Allocator));

        VulkanDeletionQueue::Add([&]() {
            Shutdown();
        });
    }

    void VulkanAllocator::Shutdown()
    {
        VmaTotalStatistics stats;
		vmaCalculateStatistics(s_Allocator, &stats);
		PXL_LOG_INFO(LogArea::Vulkan, "(VMA) Total device memory leaked: {} bytes.", stats.total.statistics.allocationBytes); // NOTE: unsure if this even works, maybe it needs to be enabled somehow?
        
        if (s_Allocator)
        {
            vmaDestroyAllocator(s_Allocator);
            s_Allocator = nullptr;
        }
    }
}