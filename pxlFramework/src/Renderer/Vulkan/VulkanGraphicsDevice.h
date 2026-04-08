#pragma once

#include <volk/volk.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vma/vk_mem_alloc.h>

#include "Core/Window.h"
#include "Renderer/GPUResource.h"
#include "Renderer/GraphicsDevice.h"

namespace pxl
{
    class VulkanGraphicsDevice : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(const GraphicsDeviceSpecs& specs, VkInstance instance);
        ~VulkanGraphicsDevice();

        virtual std::shared_ptr<GPUBuffer> CreateBuffer(const GPUBufferSpecs& specs) override;
        virtual std::shared_ptr<Texture> CreateTexture(const TextureSpecs& specs) override;
        virtual std::shared_ptr<Shader> CreateShader(const ShaderSpecs& specs) override;
        virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) override;

        virtual std::shared_ptr<ImGuiRenderer> CreateImGuiRenderer(const ImGuiSpecs& specs) override;

        virtual void Submit(const GraphicsContext& context, uint32_t frameIndex) override;

        virtual void WaitOnFrame(uint32_t frameIndex) override;

        virtual void Present() override;

        virtual void OnWindowResize() override;

        virtual void FreeResources() override;

        virtual void SetVerticalSync(bool value) override;



        virtual const GraphicsDeviceSpecs& GetSpecs() const override { return m_Specs; }
        virtual const GraphicsDeviceLimits& GetLimits() const override { return m_Limits; }

        virtual const std::string& GetGPUName() const override { return m_GPUName; }
        virtual const std::string& GetDriverInfo() const override { return m_DriverInfo; }
        virtual GPUType GetGPUType() const override { return m_GPUType; }

        VkImage GetCurrentSwapchainImage() const { return m_PerImageData.at(m_SwapchainImageIndex).Image; }
        VkImageView GetCurrentSwapchainImageView() const { return m_PerImageData.at(m_SwapchainImageIndex).View; }

        VkExtent2D GetSwapchainExtent() const { return m_SwapchainExtent; }

        VkCommandBuffer GetFrameCommandBuffer(uint32_t frameIndex) const { return m_PerFrameData.at(frameIndex).CommandBuffer; }

    private:
        struct PerImageData
        {
            VkImage Image;
            VkImageView View;
            VkSemaphore RenderFinishedSemaphore;
        };

        struct PerFrameData
        {
            VkCommandPool CommandPool = VK_NULL_HANDLE;
            VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
            VkSemaphore ImageAcquiredSemaphore = VK_NULL_HANDLE;
            VkFence RenderFinishedFence = VK_NULL_HANDLE;
        };

        void SelectGPU();

        void InitSurface(const std::shared_ptr<Window>& window);
        void InitDevice();
        void InitAllocator();
        void InitSwapchain();

        void CreatePerFrameData(PerFrameData& data);
        void DestroyPerFrameData(PerFrameData& data);

        void DestroyPerImageData(PerImageData& data);

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;

        GraphicsDeviceSpecs m_Specs = {};
        GraphicsDeviceLimits m_Limits = {};

        std::string m_GPUName;
        std::string m_DriverInfo;
        GPUType m_GPUType;

        // Swapchain data
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkExtent2D m_SwapchainExtent;
        VkPresentModeKHR m_SwapchainPresentMode;
        std::vector<PerImageData> m_PerImageData;
        std::vector<PerFrameData> m_PerFrameData;
        uint32_t m_SwapchainImageIndex = 0;
        VkSurfaceFormatKHR m_SurfaceFormat;
        bool m_SwapchainInvalid = false;

        // Queue data
        std::optional<uint32_t> m_GraphicsQueueFamily;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkCommandPool m_OneTimeCommandPool = VK_NULL_HANDLE;

        // Allocated resources
        std::vector<std::weak_ptr<GPUResource>> m_Resources;
    };

    namespace VulkanUtils
    {
        inline GPUType ToGPUType(VkPhysicalDeviceType type)
        {
            switch (type)
            {
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return GPUType::Integrated;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return GPUType::Discrete;
                default:                                     return GPUType::Other;
            }
        }
    }
}