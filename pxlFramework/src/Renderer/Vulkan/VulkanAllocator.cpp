#include "VulkanAllocator.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VmaAllocator VulkanAllocator::s_Allocator = nullptr;

    void VulkanAllocator::Init(VkInstance instance, const std::shared_ptr<VulkanDevice>& device)
    {
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.physicalDevice = static_cast<VkPhysicalDevice>(device->GetPhysical());
        allocatorCreateInfo.device = static_cast<VkDevice>(device->GetLogical());
        allocatorCreateInfo.instance = instance;
        allocatorCreateInfo.pVulkanFunctions = nullptr;

        VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &s_Allocator));

        VulkanDeletionQueue::Add([&]() {
            Shutdown();
        });
    }

    void VulkanAllocator::Shutdown()
    {
        #ifdef PXL_ENABLE_LOGGING
            VmaTotalStatistics stats;
		    vmaCalculateStatistics(s_Allocator, &stats);
		    PXL_LOG_INFO(LogArea::Vulkan, "(VMA) Total device memory leaked: {} bytes.", stats.total.statistics.allocationBytes);
        #endif
        
        vmaDestroyAllocator(s_Allocator);
        s_Allocator = nullptr;
    }
}