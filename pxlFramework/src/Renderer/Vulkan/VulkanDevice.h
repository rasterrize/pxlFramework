#pragma once

#include <Volk/volk.h>

#include "Renderer/GraphicsDevice.h"
#include "VulkanHelpers.h"

namespace pxl
{
    class VulkanDevice : public GraphicsDevice
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        virtual void* GetLogical() const override { return m_LogicalDevice; }
        virtual void* GetPhysical() const override { return m_PhysicalDevice; }

        virtual void WaitIdle() const override { VK_CHECK(vkDeviceWaitIdle(m_LogicalDevice)); }
        virtual void QueueWaitIdle(QueueType queue) const override { VK_CHECK(vkQueueWaitIdle(GetQueueFromQueueType(queue))); }

        virtual const GraphicsDeviceLimits& GetLimits() const override { return m_DeviceLimits; }

        std::vector<VkCommandBuffer> AllocateCommandBuffers(QueueType queueType, VkCommandBufferLevel level, uint32_t count);

        void SubmitCommandBuffer(const VkSubmitInfo& submitInfo, QueueType queueType, VkFence signalFence = VK_NULL_HANDLE);
        void SubmitCommandBuffers(const std::vector<VkSubmitInfo>& submitInfos, QueueType queueType, VkFence signalFence);

        VkResult SubmitPresent(const VkPresentInfoKHR& presentInfo);

        VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        VkQueue GetComputeQueue() const { return VK_NULL_HANDLE; }

        uint32_t GetGraphicsQueueFamily() const { return m_GraphicsQueueFamily.value(); }
        uint32_t GetComputeQueueFamily() const { return 0; }

        VkDevice GetVkLogical() const { return m_LogicalDevice; }
        VkPhysicalDevice GetVkPhysical() const { return m_PhysicalDevice; }

        void LogDeviceLimits(); // could be CheckDeviceLimits later so I can ensure correct device compatibility

    private:
        void CreateLogicalDevice(VkPhysicalDevice gpu);

        VkQueue GetQueueFromQueueType(QueueType type) const;
        VkCommandPool GetCommandPoolFromQueueType(QueueType type) const;

    private:
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        GraphicsDeviceLimits m_DeviceLimits = {};

        std::optional<uint32_t> m_GraphicsQueueFamily;
        std::optional<uint32_t> m_ComputeQueueFamily; // TODO: unused

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_ComputeQueue = VK_NULL_HANDLE; // TODO: unused

        VkCommandPool m_GraphicsCommandPool = VK_NULL_HANDLE;
        VkCommandPool m_ComputeCommandPool = VK_NULL_HANDLE; // TODO: unused
    };
}
