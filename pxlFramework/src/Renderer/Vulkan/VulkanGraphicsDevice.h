#pragma once

#include <vma/vk_mem_alloc.h>
#include <volk/volk.h>

#include "Core/Window.h"
#include "Renderer/GPUResource.h"
#include "Renderer/GraphicsDevice.h"

namespace pxl
{
    struct VulkanFrame
    {
        VkCommandPool CommandPool = VK_NULL_HANDLE;
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

        // Signaled when an image is acquired from the swapchain
        VkSemaphore ImageAcquiredSemaphore = VK_NULL_HANDLE;

        // Signaled once all rendering commands have finished
        VkSemaphore RenderingFinishedSemaphore = VK_NULL_HANDLE;

        // Signaled once all rendering commands have finished
        VkFence RenderFinishedFence = VK_NULL_HANDLE;
    };

    class VulkanGraphicsDevice : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(const GraphicsDeviceSpecs& specs, VkInstance instance);

        virtual std::shared_ptr<GPUBuffer> CreateBuffer(const GPUBufferSpecs& specs) override;
        virtual std::shared_ptr<Texture> CreateTexture(const TextureSpecs& specs) override;
        virtual std::shared_ptr<Shader> CreateShader(const ShaderSpecs& specs) override;
        virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) override;

        virtual std::shared_ptr<ImGuiRenderer> CreateImGuiRenderer(const std::shared_ptr<Window>& window) override;

        virtual void Present() override;

        virtual void WaitIdle() const override;
        virtual void QueueWaitIdle(QueueType queueType) const override;

        virtual void FreeResources() override;

        virtual GraphicsDeviceLimits GetLimits() const override { return GraphicsDeviceLimits(); }

        void AcquireNextSwapchainImage();

        VkImage GetCurrentSwapchainImage() const { return m_SwapchainImages.at(m_SwapchainImageIndex); }
        VkImageView GetCurrentSwapchainImageView() const { return m_SwapchainViews.at(m_SwapchainImageIndex); }
        const VulkanFrame& GetCurrentFrame() const { return m_PerFrameData.at(m_SwapchainImageIndex); }

        VkExtent2D GetSwapchainExtent() const { return m_SwapchainExtent; }

        void SubmitCurrentFrame();

    private:
        void Init(const GraphicsDeviceSpecs& specs);
        void InitDevice();
        void InitAllocator();
        void InitSwapchain(const GraphicsDeviceSpecs& specs);

        void CreateFrameData(VulkanFrame& frame);
        void DestroyFrameData(VulkanFrame& frame);

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;

        // Swapchain data
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkExtent2D m_SwapchainExtent;
        VkPresentModeKHR m_SwapchainPresentMode;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainViews;
        std::vector<VulkanFrame> m_PerFrameData;
        uint32_t m_SwapchainImageIndex = 0;
        std::vector<VkSemaphore> m_RecycledSemaphores;
        VkSurfaceFormatKHR m_SurfaceFormat;

        // Queue data
        std::optional<uint32_t> m_GraphicsQueueFamily;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        // Allocated resources
        std::vector<std::weak_ptr<GPUResource>> m_Resources;
    };
}