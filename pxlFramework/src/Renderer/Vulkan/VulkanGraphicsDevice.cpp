#include "VulkanGraphicsDevice.h"

#include "VulkanBindlessTextureHandler.h"
#include "VulkanGPUBuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanImGuiRenderer.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(const GraphicsDeviceSpecs& specs, VkInstance instance)
        : m_Specs(specs), m_Instance(instance)
    {
        InitSurface(specs.Window);
        SelectGPU();
        InitDevice();
        InitAllocator();
        InitSwapchain();

        // Init frame data
        PXL_ASSERT(specs.FramesInFlightCount > 0);
        m_PerFrameData.resize(specs.FramesInFlightCount);

        for (auto& data : m_PerFrameData)
        {
            CreatePerFrameData(data);
        }
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        VK_CHECK(vkDeviceWaitIdle(m_Device));

        FreeResources();

        if (m_Allocator)
        {
            vmaDestroyAllocator(m_Allocator);
            m_Allocator = VK_NULL_HANDLE;
        }

        for (auto& frameData : m_PerFrameData)
        {
            DestroyPerFrameData(frameData);
        }

        for (auto& imageData : m_PerImageData)
        {
            DestroyPerImageData(imageData);
        }

        if (m_Swapchain)
        {
            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
            m_Swapchain = VK_NULL_HANDLE;
        }

        if (m_OneTimeCommandPool)
        {
            vkDestroyCommandPool(m_Device, m_OneTimeCommandPool, nullptr);
            m_OneTimeCommandPool = VK_NULL_HANDLE;
        }

        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = VK_NULL_HANDLE;
        }

        if (m_Surface)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
    }

    std::shared_ptr<GPUBuffer> VulkanGraphicsDevice::CreateBuffer(const GPUBufferSpecs& specs)
    {
        auto buffer = std::make_shared<VulkanGPUBuffer>(specs, m_Device, m_Allocator, m_OneTimeCommandPool, m_GraphicsQueue);
        m_Resources.push_back(buffer);
        return buffer;
    }

    std::shared_ptr<Texture> VulkanGraphicsDevice::CreateTexture(const TextureSpecs& specs)
    {
        VulkanTextureParams params = { m_Device, m_Allocator, m_OneTimeCommandPool, m_GraphicsQueue, m_Limits.MaxAnisotropicLevel };
        auto texture = std::make_shared<VulkanTexture>(specs, params);
        m_Resources.push_back(texture);
        return texture;
    }

    std::shared_ptr<Shader> VulkanGraphicsDevice::CreateShader(const ShaderSpecs& specs)
    {
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

    std::shared_ptr<TextureHandler> VulkanGraphicsDevice::CreateTextureHandler()
    {
        auto textureHandler = std::make_shared<VulkanBindlessTextureHandler>(m_Device);
        m_Resources.push_back(textureHandler);
        return textureHandler;
    }

    std::shared_ptr<ImGuiRenderer> VulkanGraphicsDevice::CreateImGuiRenderer(const ImGuiSpecs& specs)
    {
        auto imguiRenderer = std::make_shared<VulkanImGuiRenderer>(specs, m_Instance, m_GPU, m_Device, m_GraphicsQueue, m_SurfaceFormat.format, static_cast<uint32_t>(m_PerImageData.size()));
        m_Resources.push_back(imguiRenderer);
        return imguiRenderer;
    }

    void VulkanGraphicsDevice::Submit(uint32_t frameIndex)
    {
        PXL_PROFILE_SCOPE;

        // Wait at the top of pipeline before executing any commands, as we need to ensure the swapchain image is ready first
        VkPipelineStageFlags waitStage = { VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT };

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_PerFrameData.at(frameIndex).ImageAcquiredSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_PerFrameData.at(frameIndex).CommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_PerImageData.at(m_SwapchainImageIndex).RenderFinishedSemaphore;

        VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_PerFrameData.at(frameIndex).RenderFinishedFence));
    }

    void VulkanGraphicsDevice::WaitOnFrame(uint32_t frameIndex)
    {
        PXL_PROFILE_SCOPE;

        auto frame = m_PerFrameData.at(frameIndex);
        VK_CHECK(vkWaitForFences(m_Device, 1, &frame.RenderFinishedFence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(m_Device, 1, &frame.RenderFinishedFence));

        // Reset the frame's command buffer ready for a new set of commands
        if (frame.CommandPool)
        {
            VK_CHECK(vkResetCommandPool(m_Device, frame.CommandPool, 0));
        }

        if (m_SwapchainInvalid)
            InitSwapchain();

        auto result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, frame.ImageAcquiredSemaphore, nullptr, &m_SwapchainImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            InitSwapchain();
    }

    void VulkanGraphicsDevice::Present()
    {
        PXL_PROFILE_SCOPE;

        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_PerImageData.at(m_SwapchainImageIndex).RenderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_SwapchainImageIndex;

        auto result = vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            InitSwapchain();
    }

    void VulkanGraphicsDevice::OnWindowFBResize(const WindowFBResizeEvent& e)
    {
        if (e.GetSize().Width != m_SwapchainExtent.width || e.GetSize().Height != m_SwapchainExtent.height)
            m_SwapchainInvalid = true;
    }

    void VulkanGraphicsDevice::FreeResources()
    {
        VK_CHECK(vkDeviceWaitIdle(m_Device));

        for (auto& resource : std::views::reverse(m_Resources))
        {
            resource->Free();
            resource.reset();
        }

        m_Resources.clear();
    }

    void VulkanGraphicsDevice::SelectGPU()
    {
        auto gpus = VulkanUtils::GetAvailablePhysicalDevices(m_Instance);

        // Select GPU using device index if it is specified
        if (m_Specs.DeviceIndex > -1 && m_Specs.DeviceIndex < gpus.size())
        {
            if (m_Specs.DeviceIndex < gpus.size())
            {
                m_GPU = gpus.at(m_Specs.DeviceIndex);
            }
            else
            {
                PXL_LOG_WARN(LogArea::Vulkan, "Physical device index {} wasn't found, using first available GPU instead");
                m_GPU = gpus.front();
            }
        }

        // Otherwise, find a suitable GPU matching the GPU preference
        if (!m_GPU)
        {
            for (const auto& gpu : gpus)
            {
                VkPhysicalDeviceProperties2 props = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
                vkGetPhysicalDeviceProperties2(gpu, &props);

                if (props.properties.deviceType != VulkanUtils::ToVkPhysicalDeviceType(m_Specs.TypePreference))
                    continue;

                if (props.properties.apiVersion < VK_API_VERSION_1_3)
                    continue;

                // Find a queue family supporting both graphics and present operations
                auto queueFamilies = VulkanUtils::GetQueueFamilies2(gpu);
                bool foundSuitableGraphicsQueueFamily = false;
                uint32_t graphicsQueueFamilyIndex = 0;
                for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++)
                {
                    VkBool32 supportsPresent = VK_FALSE;
                    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, m_Surface, &supportsPresent));

                    if ((queueFamilies.at(i).queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && supportsPresent)
                    {
                        graphicsQueueFamilyIndex = i;
                        foundSuitableGraphicsQueueFamily = true;
                        break;
                    }
                }

                if (!foundSuitableGraphicsQueueFamily)
                    continue;

                // Query for vulkan 1.3 features
                VkPhysicalDeviceFeatures2 deviceFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
                VkPhysicalDeviceVulkan13Features vulkan13Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
                VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT };

                deviceFeatures.pNext = &vulkan13Features;
                vulkan13Features.pNext = &extendedDynamicStateFeatures;

                vkGetPhysicalDeviceFeatures2(gpu, &deviceFeatures);

                if (!vulkan13Features.dynamicRendering)
                    continue;

                if (!vulkan13Features.synchronization2)
                    continue;

                if (!extendedDynamicStateFeatures.extendedDynamicState)
                    continue;

                m_GPU = gpu;
                m_GraphicsQueueFamily = graphicsQueueFamilyIndex;
                break;
            }
        }
    }

    void VulkanGraphicsDevice::InitSurface(const std::shared_ptr<Window>& window)
    {
        m_Surface = window->CreateVKSurface(m_Instance);
    }

    void VulkanGraphicsDevice::InitDevice()
    {
        // Enable the specific features we will use
        VkPhysicalDeviceFeatures2 enabledDeviceFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

        // Enable Vulkan 1.0 features
        enabledDeviceFeatures.features.samplerAnisotropy = VK_TRUE;
        enabledDeviceFeatures.features.fillModeNonSolid = VK_TRUE;

        // Enable Vulkan 1.1 features
        VkPhysicalDeviceVulkan11Features enabledVulkan11Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };

        // Enable Vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features enabledVulkan12Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        enabledVulkan12Features.bufferDeviceAddress = VK_TRUE;
        enabledVulkan12Features.descriptorBindingVariableDescriptorCount = VK_TRUE;
        enabledVulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        enabledVulkan12Features.runtimeDescriptorArray = VK_TRUE;

        // Enable Vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features enabledVulkan13Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        enabledVulkan13Features.dynamicRendering = VK_TRUE;
        enabledVulkan13Features.synchronization2 = VK_TRUE;

        // Enable extended dynamic state features
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT enabledExtendedDynamicStateFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT };
        enabledExtendedDynamicStateFeatures.extendedDynamicState = VK_TRUE;

        enabledVulkan11Features.pNext = &enabledExtendedDynamicStateFeatures;
        enabledVulkan12Features.pNext = &enabledVulkan11Features;
        enabledVulkan13Features.pNext = &enabledVulkan12Features;
        enabledDeviceFeatures.pNext = &enabledVulkan13Features;

        // The swapchain extension is needed to display to the screen
        std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        auto availableExtensions = VulkanUtils::GetDeviceExtensions(m_GPU);

#if defined(PXL_DEBUG) && defined(PXL_ENABLE_LOGGING)
        PXL_LOG_INFO(LogArea::Vulkan, "Required device extensions selected:")
        for (const auto& extensionName : requiredExtensions)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "- {}", extensionName);
        }
#endif

        if (!VulkanUtils::ValidateExtensions(requiredExtensions, availableExtensions))
            throw std::runtime_error("Failed to validate all device extensions");

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

        // Init one time command pool
        VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.queueFamilyIndex = static_cast<uint32_t>(m_GraphicsQueueFamily.value());
        VK_CHECK(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_OneTimeCommandPool));

        // Load GPU properties
        VkPhysicalDeviceDriverProperties driverProps = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES };
        VkPhysicalDeviceProperties2 gpuProps = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        gpuProps.pNext = &driverProps;

        vkGetPhysicalDeviceProperties2(m_GPU, &gpuProps);

        m_GPUName = gpuProps.properties.deviceName;
        m_GPUType = VulkanUtils::ToGPUType(gpuProps.properties.deviceType);
        m_DriverInfo = driverProps.driverInfo;
        m_Limits.MaxAnisotropicLevel = gpuProps.properties.limits.maxSamplerAnisotropy;

#ifdef PXL_DEBUG
        PXL_LOG_INFO(LogArea::Vulkan, "Selected GPU: {}", m_GPUName);
        PXL_LOG_INFO(LogArea::Vulkan, "- Driver info: {}", m_DriverInfo);
#endif
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

    void VulkanGraphicsDevice::InitSwapchain()
    {
        // Graphics device must not be performing any operations when we recreate the swapchain
        VK_CHECK(vkDeviceWaitIdle(m_Device));

        // Select a suitable surface format based on what the surface supports
        auto surfaceFormats = VulkanUtils::GetSurfaceFormats(m_GPU, m_Surface);
        auto surfaceCapabilities = VulkanUtils::GetSurfaceCapabilities(m_GPU, m_Surface);
        auto surfacePresentModes = VulkanUtils::GetSurfacePresentModes(m_GPU, m_Surface);

        // Determine the swapchain extent
        // NOTE: 0xFFFFFFFF is a special value indicating the surface size will be determined by the swapchain
        if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
        {
            auto size = m_Specs.Window->GetFramebufferSize();
            m_SwapchainExtent = { size.Width, size.Height };
        }
        else
        {
            m_SwapchainExtent = surfaceCapabilities.currentExtent;
        }

        // Determine a suitable surface format
        m_SurfaceFormat = VulkanUtils::GetSuitableSurfaceFormat(surfaceFormats);

        // Determine a suitable present mode
        if (m_Specs.VerticalSync)
        {
            m_SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
        else
        {
            std::vector<VkPresentModeKHR> modePriority;
            if (m_Specs.AllowTearing)
                modePriority = { VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_MAILBOX_KHR };
            else
                modePriority = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR };

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

        // Determine the minimum image count
        auto minCount = surfaceCapabilities.minImageCount;
        auto maxCount = surfaceCapabilities.maxImageCount;
        auto desiredImageCount = minCount;

        if (m_Specs.TripleBuffering)
        {
            // A max image count of 0 means no limit
            if (maxCount >= 3 || maxCount == 0)
            {
                // Triple buffering is supported
                desiredImageCount = std::max(3, static_cast<int>(minCount));
                m_Limits.TripleBufferingSupported = true;
            }
            else
            {
                PXL_LOG_WARN(LogArea::Vulkan, "Triple buffering not supported by window surface");
            }
        }

        // Triple buffering may be forced by the graphics device
        m_Limits.ForcedTripleBuffering = minCount > 2;

        if (!m_Specs.TripleBuffering && m_Limits.ForcedTripleBuffering)
        {
            PXL_LOG_WARN(LogArea::Vulkan, "Triple buffering not enabled but is forced by the driver/OS");
        }

        // Create the swapchain
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
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = m_SwapchainPresentMode;
        swapchainInfo.clipped = true;
        swapchainInfo.oldSwapchain = oldSwapchain;

        VK_CHECK(vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain));
        m_SwapchainInvalid = false;

        // If we used the old swapchain, we must destroy it and its resources
        if (oldSwapchain)
        {
            for (auto& data : m_PerImageData)
            {
                DestroyPerImageData(data);
            }

            vkDestroySwapchainKHR(m_Device, oldSwapchain, nullptr);
        }

        auto images = VulkanUtils::GetSwapchainImages(m_Device, m_Swapchain);

        m_PerImageData.clear();
        m_PerImageData.resize(images.size());

        // Init per image data
        for (size_t i = 0; i < images.size(); i++)
        {
            m_PerImageData[i].Image = images.at(i);

            VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            viewInfo.image = m_PerImageData[i].Image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_SurfaceFormat.format;
            viewInfo.subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VK_CHECK(vkCreateImageView(m_Device, &viewInfo, nullptr, &m_PerImageData.at(i).View));

            m_PerImageData[i].RenderFinishedSemaphore = VulkanUtils::CreateSemaphore(m_Device);
        }

#ifdef PXL_DEBUG
        PXL_LOG_INFO(LogArea::Vulkan, "Vulkan swapchain created");
        PXL_LOG_INFO(LogArea::Vulkan, "- Desired image count: {}", desiredImageCount);
        PXL_LOG_INFO(LogArea::Vulkan, "- Extent: {}, {}", m_SwapchainExtent.width, m_SwapchainExtent.height);
        PXL_LOG_INFO(LogArea::Vulkan, "- Actual image count: {}", m_PerImageData.size());
        PXL_LOG_INFO(LogArea::Vulkan, "- Vertical sync: {}", m_Specs.VerticalSync);
        PXL_LOG_INFO(LogArea::Vulkan, "- Allow tearing: {}", m_Specs.AllowTearing);
        PXL_LOG_INFO(LogArea::Vulkan, "- Triple buffering: {}", m_Specs.TripleBuffering);
#endif
    }

    void VulkanGraphicsDevice::DestroyPerImageData(PerImageData& data)
    {
        if (data.RenderFinishedSemaphore)
        {
            vkDestroySemaphore(m_Device, data.RenderFinishedSemaphore, nullptr);
            data.RenderFinishedSemaphore = VK_NULL_HANDLE;
        }

        if (data.View)
        {
            vkDestroyImageView(m_Device, data.View, nullptr);
            data.View = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsDevice::CreatePerFrameData(PerFrameData& data)
    {
        // Create command pool
        VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // Indicates the command buffers will be short-lived, and may affect underlying memory allocation
        poolInfo.queueFamilyIndex = static_cast<uint32_t>(m_GraphicsQueueFamily.value());

        VK_CHECK(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &data.CommandPool));

        // Allocate primary command buffer
        VkCommandBufferAllocateInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        cmdBufferInfo.commandPool = data.CommandPool;
        cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufferInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdBufferInfo, &data.CommandBuffer));

        // Create queue submit fence
        data.RenderFinishedFence = VulkanUtils::CreateFence(m_Device, true);

        // Create image acquired semaphore
        data.ImageAcquiredSemaphore = VulkanUtils::CreateSemaphore(m_Device);
    }

    void VulkanGraphicsDevice::DestroyPerFrameData(PerFrameData& data)
    {
        if (data.CommandBuffer)
        {
            vkFreeCommandBuffers(m_Device, data.CommandPool, 1, &data.CommandBuffer);
            data.CommandBuffer = VK_NULL_HANDLE;
        }

        if (data.CommandPool)
        {
            vkDestroyCommandPool(m_Device, data.CommandPool, nullptr);
            data.CommandPool = VK_NULL_HANDLE;
        }

        if (data.RenderFinishedFence)
        {
            vkDestroyFence(m_Device, data.RenderFinishedFence, nullptr);
            data.RenderFinishedFence = VK_NULL_HANDLE;
        }

        if (data.ImageAcquiredSemaphore)
        {
            vkDestroySemaphore(m_Device, data.ImageAcquiredSemaphore, nullptr);
            data.ImageAcquiredSemaphore = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsDevice::SetVerticalSync(bool value)
    {
        m_Specs.VerticalSync = value;
        m_SwapchainInvalid = true;
    }

    void VulkanGraphicsDevice::SetTripleBuffering(bool value)
    {
        m_Specs.TripleBuffering = value;
        m_SwapchainInvalid = true;
    }

    void VulkanGraphicsDevice::SetAllowTearing(bool value)
    {
        m_Specs.AllowTearing = value;
        m_SwapchainInvalid = true;
    }
}
