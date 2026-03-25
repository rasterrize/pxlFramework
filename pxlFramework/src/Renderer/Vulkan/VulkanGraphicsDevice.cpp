#include "VulkanGraphicsDevice.h"

#include "VulkanGPUBuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanImGuiRenderer.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(const GraphicsDeviceSpecs& specs, VkInstance instance)
        : m_Instance(instance)
    {
        m_Surface = specs.RendererConfig.Window->CreateVKSurface(m_Instance);

        // Find a suitable physical device (gpu)
        auto gpus = VulkanUtils::GetAvailablePhysicalDevices(m_Instance);
        for (const auto& gpu : gpus)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(gpu, &props);

            if (props.deviceType != VulkanUtils::ToVkPhysicalDeviceType(specs.Preference))
                continue;

            if (props.apiVersion < VK_API_VERSION_1_3)
                continue;

            auto queueFamilies = VulkanUtils::GetQueueFamilies(gpu);
            for (size_t i = 0; i < queueFamilies.size(); i++)
            {
                VkBool32 supportsPresent = VK_FALSE;
                VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, m_Surface, &supportsPresent));

                if ((queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) && supportsPresent)
                {
                    m_GraphicsQueueFamily = i;
                    break;
                }
            }

            if (!m_GraphicsQueueFamily.has_value())
                continue;

            m_GPU = gpu;
            break;
        }

        if (!m_GPU)
            throw std::runtime_error("Failed to find suitable GPU for vulkan");

        // Query for vulkan 1.3 features
        VkPhysicalDeviceFeatures2 deviceFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceVulkan13Features vulkan13Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT };

        deviceFeatures.pNext = &vulkan13Features;
        vulkan13Features.pNext = &extendedDynamicStateFeatures;

        vkGetPhysicalDeviceFeatures2(m_GPU, &deviceFeatures);

        if (!vulkan13Features.dynamicRendering)
            throw std::runtime_error("GPU doesn't support dynamic rendering feature");

        if (!vulkan13Features.synchronization2)
            throw std::runtime_error("GPU doesn't support synchronization2 feature");

        if (!extendedDynamicStateFeatures.extendedDynamicState)
            throw std::runtime_error("GPU doesn't support extended dynamic state feature");

        Init(specs);
    }

    std::shared_ptr<GPUBuffer> VulkanGraphicsDevice::CreateBuffer(const GPUBufferSpecs& specs)
    {
        auto buffer = std::make_shared<VulkanGPUBuffer>(specs, m_Device, m_Allocator);
        m_Resources.push_back(buffer);
        return buffer;
    }

    std::shared_ptr<Texture> VulkanGraphicsDevice::CreateTexture(const TextureSpecs& specs)
    {
        PXL_LOG_ERROR(LogArea::Vulkan, "Vulkan textures are not implemented yet");
        return nullptr;
    }

    std::shared_ptr<Shader> VulkanGraphicsDevice::CreateShader(const ShaderSpecs& specs)
    {
        // TODO: probably move this elsewhere
        if (!std::filesystem::exists(specs.FilePath))
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create Shader, shader file '{}' does not exist", specs.FilePath.string());
            return nullptr;
        }

        auto ext = specs.FilePath.extension();
        if (ext != ".glsl" && ext != ".spv")
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create shader, File extension '{}' isn't supported");
            return nullptr;
        }

        auto shader = std::make_shared<VulkanShader>(specs, m_Device);
        m_Resources.push_back(shader);
        return shader;
    }

    std::shared_ptr<GraphicsPipeline> VulkanGraphicsDevice::CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs)
    {
        auto pipeline = std::make_shared<VulkanGraphicsPipeline>(specs, m_Device, m_SurfaceFormat.format);
        m_Resources.push_back(pipeline);
        return pipeline;
    }

    std::shared_ptr<ImGuiRenderer> VulkanGraphicsDevice::CreateImGuiRenderer(const std::shared_ptr<Window>& window)
    {
        auto imguiRenderer = std::make_shared<VulkanImGuiRenderer>(window, m_Instance, m_GPU, m_Device, m_GraphicsQueue, m_SurfaceFormat.format, m_SwapchainImages.size());
        m_Resources.push_back(imguiRenderer);
        return imguiRenderer;
    }

    void VulkanGraphicsDevice::Present()
    {
        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_PerFrameData.at(m_SwapchainImageIndex).RenderingFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_SwapchainImageIndex;

        vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);
    }

    void VulkanGraphicsDevice::WaitIdle() const
    {
        VK_CHECK(vkDeviceWaitIdle(m_Device));
    }

    void VulkanGraphicsDevice::QueueWaitIdle(QueueType queueType) const
    {
        VkQueue queue = VK_NULL_HANDLE;
        switch (queueType)
        {
            case QueueType::Graphics: queue = m_GraphicsQueue; break;
            default:                  break;
        }

        VK_CHECK(vkQueueWaitIdle(queue));
    }

    void VulkanGraphicsDevice::FreeResources()
    {
        WaitIdle();

        for (auto& resource : std::views::reverse(m_Resources))
        {
            resource.lock()->Free();
        }

        if (m_Allocator)
        {
            vmaDestroyAllocator(m_Allocator);
            m_Allocator = VK_NULL_HANDLE;
            PXL_LOG_INFO(LogArea::Vulkan, "Vulkan allocator destroyed");
        }

        for (auto& semaphore : m_RecycledSemaphores)
        {
            vkDestroySemaphore(m_Device, semaphore, nullptr);
        }

        m_RecycledSemaphores.clear();

        for (auto& frame : m_PerFrameData)
        {
            DestroyFrameData(frame);
        }

        for (const auto& view : m_SwapchainViews)
        {
            vkDestroyImageView(m_Device, view, nullptr);
        }

        if (m_Swapchain)
        {
            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
            m_Swapchain = nullptr;
            PXL_LOG_INFO(LogArea::Vulkan, "Vulkan swapchain destroyed");
        }

        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = VK_NULL_HANDLE;
            PXL_LOG_INFO(LogArea::Vulkan, "Vulkan device destroyed");
        }

        if (m_Surface)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
            PXL_LOG_INFO(LogArea::Vulkan, "Vulkan surface destroyed");
        }
    }

    void VulkanGraphicsDevice::SubmitCurrentFrame()
    {
        auto& frame = m_PerFrameData.at(m_SwapchainImageIndex);

        // Wait at the top of pipeline before executing any commands, as we need to ensure the swapchain image is ready first
        VkPipelineStageFlags waitStage = { VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT };

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &frame.ImageAcquiredSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.CommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &frame.RenderingFinishedSemaphore;

        VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, frame.RenderFinishedFence));
    }

    void VulkanGraphicsDevice::Init(const GraphicsDeviceSpecs& specs)
    {
        InitDevice();
        InitSwapchain(specs);
        InitAllocator();
    }

    void VulkanGraphicsDevice::InitDevice()
    {
        // Enable the specific features we will use
        VkPhysicalDeviceFeatures2 enabledDeviceFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceVulkan13Features enabledVulkan13Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT enabledExtendedDynamicStateFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT };

        enabledVulkan13Features.dynamicRendering = VK_TRUE;
        enabledVulkan13Features.synchronization2 = VK_TRUE;
        enabledExtendedDynamicStateFeatures.extendedDynamicState = VK_TRUE;

        enabledVulkan13Features.pNext = &enabledExtendedDynamicStateFeatures;
        enabledDeviceFeatures.pNext = &enabledVulkan13Features;

        // The swapchain extension is needed to display to the screen
        std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        auto availableExtensions = VulkanUtils::GetDeviceExtensions(m_GPU);

        if (VulkanUtils::ValidateExtensions(requiredExtensions, availableExtensions))
            std::runtime_error("Failed to validate all device extensions");

        // The graphics queue will be the highest priority (for now)
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo graphicsQueueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        graphicsQueueInfo.queueFamilyIndex = m_GraphicsQueueFamily.value();
        graphicsQueueInfo.queueCount = 1;
        graphicsQueueInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceInfo.pNext = &enabledDeviceFeatures;
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.pQueueCreateInfos = &graphicsQueueInfo;
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VK_CHECK(vkCreateDevice(m_GPU, &deviceInfo, nullptr, &m_Device));
        volkLoadDevice(m_Device);

        PXL_LOG_INFO(LogArea::Vulkan, "Vulkan device created");

        // Get the newly created graphics queue
        vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily.value(), 0, &m_GraphicsQueue);
    }

    void VulkanGraphicsDevice::InitAllocator()
    {
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        allocatorCreateInfo.physicalDevice = m_GPU;
        allocatorCreateInfo.device = m_Device;
        allocatorCreateInfo.instance = m_Instance;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;

        VmaVulkanFunctions funcs;
        VK_CHECK(vmaImportVulkanFunctionsFromVolk(&allocatorCreateInfo, &funcs));
        allocatorCreateInfo.pVulkanFunctions = &funcs;

        VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator));
        PXL_LOG_INFO(LogArea::Vulkan, "Vulkan allocator created");
    }

    void VulkanGraphicsDevice::InitSwapchain(const GraphicsDeviceSpecs& specs)
    {
        // Select a suitable surface format based on what the surface supports
        auto surfaceFormats = VulkanUtils::GetSurfaceFormats(m_GPU, m_Surface);
        auto surfaceCapabilities = VulkanUtils::GetSurfaceCapabilities(m_GPU, m_Surface);
        auto surfacePresentModes = VulkanUtils::GetSurfacePresentModes(m_GPU, m_Surface);

        // Determine the swapchain extent
        // NOTE: 0xFFFFFFFF is a special value indicating the surface size will be determined by the swapchain
        if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
        {
            auto size = specs.RendererConfig.Window->GetFramebufferSize();
            m_SwapchainExtent = { size.Width, size.Height };
        }
        else
        {
            m_SwapchainExtent = surfaceCapabilities.currentExtent;
        }

        // Determine a suitable surface format
        // TODO: verify this covers all bases
        m_SurfaceFormat = VulkanUtils::GetSuitableSurfaceFormat(surfaceFormats);

        // Determine a suitable present mode
        if (specs.RendererConfig.VerticalSync)
        {
            m_SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
        else
        {
            std::array<VkPresentModeKHR, 2> modePriority = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR };

            bool foundSuitableMode = false;
            for (const auto& mode : modePriority)
            {
                if (VulkanUtils::ValidatePresentMode(mode, surfacePresentModes))
                {
                    foundSuitableMode = true;
                    m_SwapchainPresentMode = mode;
                    break;
                }
            }

            if (!foundSuitableMode)
            {
                PXL_LOG_WARN(LogArea::Vulkan, "Failed to find suitable non-vsync present mode, forcing FIFO present mode instead");
                m_SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
            }
        }

        // TODO: Check if triple buffering is supported first
        // Determine the minimum swapchain image count
        // auto desiredImageCount = specs.RendererConfig.TripleBuffering
        //     ? surfaceCapabilities.minImageCount + 2
        //     : surfaceCapabilities.minImageCount + 1;
        auto desiredImageCount = surfaceCapabilities.minImageCount;

        // Cap the image count if it exceeds the surfaces max capable image count
        // NOTE: A maxImageCount of 0 means there is no limit
        if (surfaceCapabilities.maxImageCount != 0 && desiredImageCount > surfaceCapabilities.maxImageCount)
            desiredImageCount = surfaceCapabilities.maxImageCount;

        auto oldSwapchain = m_Swapchain;

        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = desiredImageCount;
        swapchainInfo.imageFormat = m_SurfaceFormat.format;
        swapchainInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        swapchainInfo.imageExtent = m_SwapchainExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: verify if this is more important than I think
        swapchainInfo.presentMode = m_SwapchainPresentMode;
        swapchainInfo.clipped = true;
        swapchainInfo.oldSwapchain = oldSwapchain;

        VK_CHECK(vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain));

        // If we used the old swapchain, we must destroy it and its resources
        if (oldSwapchain)
        {
            for (auto& frame : m_PerFrameData)
            {
                DestroyFrameData(frame);
            }

            for (const auto& view : m_SwapchainViews)
            {
                vkDestroyImageView(m_Device, view, nullptr);
            }

            m_SwapchainViews.clear();

            vkDestroySwapchainKHR(m_Device, oldSwapchain, nullptr);
        }

        m_SwapchainImages = VulkanUtils::GetSwapchainImages(m_Device, m_Swapchain);

        // Create an image view for each swapchain image
        for (const auto& image : m_SwapchainImages)
        {
            VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            viewInfo.image = image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_SurfaceFormat.format;
            viewInfo.subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VkImageView imageView;
            VK_CHECK(vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView));

            m_SwapchainViews.push_back(imageView);
        }

        m_PerFrameData.clear();
        m_PerFrameData.resize(m_SwapchainImages.size());

        // Create per frame data for each swapchain image
        for (size_t i = 0; i < m_PerFrameData.size(); i++)
        {
            CreateFrameData(m_PerFrameData.at(i));
        }
    }

    void VulkanGraphicsDevice::CreateFrameData(VulkanFrame& frame)
    {
        // Create command pool
        VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // Indicates the command buffers will be short-lived, and may affect underlying memory allocation
        poolInfo.queueFamilyIndex = static_cast<uint32_t>(m_GraphicsQueueFamily.value());

        VK_CHECK(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &frame.CommandPool));

        // Allocate primary command buffer
        VkCommandBufferAllocateInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        cmdBufferInfo.commandPool = frame.CommandPool;
        cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufferInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdBufferInfo, &frame.CommandBuffer));

        // Create queue submit fence
        frame.RenderFinishedFence = VulkanUtils::CreateFence(m_Device, true);

        // Create image acquired semaphore
        frame.ImageAcquiredSemaphore = VulkanUtils::CreateSemaphore(m_Device);

        // Create rendering finished semaphore
        frame.RenderingFinishedSemaphore = VulkanUtils::CreateSemaphore(m_Device);
    }

    void VulkanGraphicsDevice::DestroyFrameData(VulkanFrame& frame)
    {
        if (frame.CommandBuffer)
        {
            vkFreeCommandBuffers(m_Device, frame.CommandPool, 1, &frame.CommandBuffer);
            frame.CommandBuffer = VK_NULL_HANDLE;
        }

        if (frame.CommandPool)
        {
            vkDestroyCommandPool(m_Device, frame.CommandPool, nullptr);
            frame.CommandPool = VK_NULL_HANDLE;
        }

        if (frame.RenderFinishedFence)
        {
            vkDestroyFence(m_Device, frame.RenderFinishedFence, nullptr);
            frame.RenderFinishedFence = VK_NULL_HANDLE;
        }

        if (frame.ImageAcquiredSemaphore)
        {
            vkDestroySemaphore(m_Device, frame.ImageAcquiredSemaphore, nullptr);
            frame.ImageAcquiredSemaphore = VK_NULL_HANDLE;
        }

        if (frame.RenderingFinishedSemaphore)
        {
            vkDestroySemaphore(m_Device, frame.RenderingFinishedSemaphore, nullptr);
            frame.RenderingFinishedSemaphore = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsDevice::AcquireNextSwapchainImage()
    {
        VkSemaphore newImageAcquiredSemaphore;

        // Use recycled semaphores if they're available
        if (m_RecycledSemaphores.empty())
        {
            newImageAcquiredSemaphore = VulkanUtils::CreateSemaphore(m_Device);
        }
        else
        {
            newImageAcquiredSemaphore = m_RecycledSemaphores.back();
            m_RecycledSemaphores.pop_back();
        }

        auto result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, newImageAcquiredSemaphore, nullptr, &m_SwapchainImageIndex);

        if (result != VK_SUCCESS)
        {
            m_RecycledSemaphores.push_back(newImageAcquiredSemaphore);
            return;
        }

        // Wait for any fences of the newly acquired image
        auto frame = m_PerFrameData.at(m_SwapchainImageIndex);
        auto queueSubmitFence = frame.RenderFinishedFence;
        if (queueSubmitFence != VK_NULL_HANDLE)
        {
            VK_CHECK(vkWaitForFences(m_Device, 1, &queueSubmitFence, true, UINT64_MAX));
            VK_CHECK(vkResetFences(m_Device, 1, &queueSubmitFence));
        }

        // Reset the frame's command buffer ready for a new set of commands
        if (frame.CommandPool)
        {
            vkResetCommandPool(m_Device, frame.CommandPool, 0);
        }

        // Recycle the old semaphore
        VkSemaphore oldSemaphore = m_PerFrameData.at(m_SwapchainImageIndex).ImageAcquiredSemaphore;

        if (oldSemaphore)
        {
            m_RecycledSemaphores.push_back(oldSemaphore);
        }

        m_PerFrameData.at(m_SwapchainImageIndex).ImageAcquiredSemaphore = newImageAcquiredSemaphore;
    }
}
