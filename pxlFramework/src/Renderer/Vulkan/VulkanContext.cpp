#include "VulkanContext.h"

#include <GLFW/glfw3.h>

#include "VulkanErrorCheck.h"
#include "../Shader.h"

// TEMP
#include "../../Utils/FileLoader.h"

#include <vulkan/vk_enum_string_helper.h>

namespace pxl
{
    VulkanContext::VulkanContext(const std::shared_ptr<Window>& window)
        : m_WindowHandle(window)
    {
        Init();
    }

    VulkanContext::~VulkanContext()
    {
        Shutdown();
    }

    void VulkanContext::Init()
    {
        VkResult result;

        // Create Vulkan Instance
        // Get the extensions required for Vulkan to work with GLFW (should retrieve VK_KHR_SURFACE and platform specific extensions (VK_KHR_win32_SURFACE))
        auto glfwExtensions = m_WindowHandle->GetVKRequiredInstanceExtensions();

        // Select from available layers
        auto availableLayers = GetAvailableLayers();
        auto selectedLayers = std::vector<const char*>();

        #define PXL_DEBUG // TODO: Define this in the build system
        #ifdef PXL_DEBUG
            auto validationLayers = GetValidationLayers(availableLayers);
            selectedLayers = validationLayers;
        #endif

        // Create Vulkan Instance with specified extensions and layers
        if (!CreateInstance(glfwExtensions, selectedLayers))
            return;

        // Get the window surface from the window
        m_Surface = m_WindowHandle->CreateVKWindowSurface(m_Instance);

        // Get available physical devices
        auto physicalDevices = GetAvailablePhysicalDevices();

        if (physicalDevices.size() < 1)
            return;
        
        // Select GPU (assigns m_GPU respectively)
        if (!SelectFirstDiscreteGPU(physicalDevices))
        {
            if (!SelectFirstVKCapableGPU(physicalDevices))
            {
                Logger::LogError("Failed to select any GPU for Vulkan");
                return;
            }
        }

        // Select Queue Families
        auto queueFamilies = GetQueueFamilies(m_GPU);
        m_GraphicsQueueFamilyIndex = GetSuitableGraphicsQueueFamily(queueFamilies);

        // Create Logical Device for selected Physical Device
        if (!CreateLogicalDevice(m_GPU))
            return;

        // Retrieve the selected queue handles (graphics and present for now)
        if (!RetrieveQueueHandles())
            return;

        // Create swap chain with specified surface
        if (!CreateSwapchain(m_Surface))
            return;

        // TODO: Render passes / Graphics Pipeline / Shaders
        auto vertBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/vert.spv");
        auto fragBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/frag.spv");
        m_Shader = pxl::Shader::Create(m_Device, vertBin, fragBin);

        m_Renderpass = std::make_shared<VulkanRenderPass>(m_Device, m_SurfaceFormat.format); // should get the format of the swapchain not the surface

        m_Pipeline = std::make_shared<VulkanGraphicsPipeline>(m_Device, m_Shader, m_Renderpass);

        if (!CreateFramebuffers(m_Renderpass->GetVKRenderPass()))
            return;

        if (!CreateSyncObjects())
            return;

    }

    void VulkanContext::Shutdown()
    {
        // Destroy VK objects
        if (m_Swapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

        if (m_Surface != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

        if (m_Device != VK_NULL_HANDLE)
            vkDestroyDevice(m_Device, nullptr);

        if (m_Instance != VK_NULL_HANDLE)
            vkDestroyInstance(m_Instance, nullptr);
    }

    void VulkanContext::Present()
    {
        // Acquire the next available image in the swapchain so we can d
        //uint32_t imageIndex;
        //vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, imageAvailableSemaphore, imageAvailableFence, &imageIndex);

        VkResult result;

        uint32_t imageIndex;

        // Synchronization
        vkWaitForFences(m_Device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX); // using UINT64_MAX pretty much means an infinite timeout (18 quintillion nanoseconds = 584 years)
        vkResetFences(m_Device, 1, &m_InFlightFence); // reset the fence to unsignalled state

        // Acquire image from the swapchain
        vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        // Record commands
        vkResetCommandBuffer(m_CommandBuffer, 0);
        RecordCommands(imageIndex);

        // Submit the command buffer
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore }; // The semaphores to wait before execution
        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // which stages of the pipeline to wait on
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores; // semaphores to wait on before execution
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores; // semaphores to signal when finished

        // Submit the command buffer to the graphics queue (the command buffers in question are located in the submit info)
        // if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to submit draw command buffer!");
        // }

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence);

        VkSwapchainKHR swapChains[] = { m_Swapchain };
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores; // wait for the command buffers to finish executing (rendering) to finish before presenting
        presentInfo.pResults = nullptr; // useful for error checking when using multiple swap chains
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        // Queue presentation
        vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);

    }
    
    void VulkanContext::SetVSync(bool value)
    {
        // Don't update VSync if it already is the specified value. This is important for vulkan since the entire swapchain needs to be recreated.
        if (m_VSync != value) // lambda possible here I think
        {
            if (value)
                m_SwapchainData.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
            else
                m_SwapchainData.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

            m_VSync = value;
        }
        else
        {
            return;
        }

        // TODO: recreate swap chain
        // RecreateSwapchain(); // or maybe CreateSwapchain() again but it checks if the swapchain already exists or not.
    }

    bool VulkanContext::CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
    {
        VkResult result;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_3; // should check for available version of vulkan if possible

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = extensions.size();
        instanceInfo.ppEnabledExtensionNames = extensions.data();
        instanceInfo.enabledLayerCount = layers.size();
        instanceInfo.ppEnabledLayerNames = layers.data();

        result = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);
        CheckVkResult(result);

        if (!m_Instance)
        {
            Logger::LogError("Failed to create Vulkan instance");
            return false;
        }

        // Logging
        std::string apiVersion;
        switch (appInfo.apiVersion)
        {
            case VK_API_VERSION_1_0:
                apiVersion = "1.0";
                break;
            case VK_API_VERSION_1_1:
                apiVersion = "1.1";
                break;
            case VK_API_VERSION_1_2:
                apiVersion = "1.2";
                break;
            case VK_API_VERSION_1_3:
                apiVersion = "1.3";
                break;
                
        }

        Logger::LogInfo("VK Instance Info:");
        Logger::LogInfo("   Vulkan API Version: " + apiVersion);
        Logger::LogInfo("   " + std::to_string(extensions.size()) + " enabled extensions: ");
        
        for (auto& extension : extensions)
        {
            Logger::LogInfo("   - " + std::string(extension));
        }

        Logger::LogInfo("   " + std::to_string(layers.size()) + " enabled layers:");

        for (auto& layer : layers)
        {
            Logger::LogInfo("   - " + std::string(layer));
        }

        return true;
    }

    bool VulkanContext::CreateLogicalDevice(const VkPhysicalDevice& gpu)
    {
        if (gpu == VK_NULL_HANDLE)
        {
            Logger::LogError("Failed to create logical device, physical device was null pointer");
            return false;
        }

        // Specify Device Queue Create Infos
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        uint32_t queueCount = 0;

        if (m_GraphicsQueueFamilyIndex != -1)
        {
            float queuePriority = 1.0f;
            VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
            graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex.value();
            graphicsQueueCreateInfo.queueCount = 1;
            graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(graphicsQueueCreateInfo);
        }

        // TODO: Specify selected device features
        VkPhysicalDeviceFeatures deviceFeatures = {};
        // GetGPUFeatures()

        std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" }; // necessary for games

        auto availableExtensions = GetDeviceExtensions(m_GPU);

        uint32_t enabledExtensionCount = 0;

        // Check support

        for (const auto& extension : deviceExtensions)
        {
           for (const auto& availableExtension : availableExtensions)
            {
                if (strcmp(extension, availableExtension.extensionName) == 0) 
                {
                    enabledExtensionCount++;
                    break;
                } 
            }
        }

        if (enabledExtensionCount != deviceExtensions.size())
        {
            Logger::LogError("Failed to enable all selected device extensions");
            return false;
        }

        // Specify Device Create Info
        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());;
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(gpu, &deviceInfo, nullptr, &m_Device);
        CheckVkResult(result);

        if (!m_Device)
        {
            Logger::LogError("Failed to create logical device");
        }
        
        Logger::LogInfo("Logical Device successfully created:");
        Logger::LogInfo("   " + std::to_string(enabledExtensionCount) + " enabled extensions: ");
        
        for (uint32_t i = 0; i < enabledExtensionCount; i++)
        {
            Logger::LogInfo("   - " + std::string(deviceExtensions[i]));
        }
        
        // TODO: expand this

        return true;
    }

    bool VulkanContext::RetrieveQueueHandles()
    {
        if (m_GraphicsQueueFamilyIndex != -1)
        {
            vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex.value(), 0, &m_GraphicsQueue);

            if (!m_GraphicsQueue)
            {
                Logger::LogError("Failed to retrieve queue handles");
                return false;
            }
            
            return true;
        }

        return false;
    }

    bool VulkanContext::CreateSwapchain(const VkSurfaceKHR& surface)
    {
        VkResult result;

        // Check surface compatiblity
        auto surfaceFormats = GetSurfaceFormats(m_GPU, surface);
        auto surfacePresentModes = GetSurfacePresentModes(m_GPU, surface);
        auto surfaceCapabilities = GetSurfaceCapabilities(m_GPU, surface);

        // Select most suitable surface format
        bool foundSuitableSurfaceFormat = false;
        for (const auto& surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                m_SurfaceFormat = surfaceFormat;
                foundSuitableSurfaceFormat = true;
            }
        }

        if (!foundSuitableSurfaceFormat)
        {
            Logger::LogError("Failed to find suitable surface format for swap chain");
            return false;
        }

        // Select most suitable surface present mode
        bool foundSuitablePresentMode = false;
        for (const auto& presentMode : surfacePresentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                m_SwapchainData.PresentMode = presentMode;
                foundSuitablePresentMode = true;
                break;
            }
        }

        if (!foundSuitablePresentMode)
        {
            Logger::LogError("Failed to find suitable surface present mode for swap chain");
            m_SwapchainData.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
            return false;
        }

        // Select most suitable number of images for swapchain
        if (surfaceCapabilities.minImageCount >= 2)
        {
            if (surfaceCapabilities.maxImageCount >= 3)
            {
                m_SwapchainData.ImageCount = 3; // Triple buffering
            }
            else
            {
                m_SwapchainData.ImageCount = 2; // Double buffering
            }
        }
        else
        {
            Logger::LogError("Selected surface for swapchain must support more than 2 images");
            return false;
        }

        // TODO: TEMP - This is just so I can get Vulkan working
        m_SwapchainData.ImageCount = 2;
        m_SwapchainData.PresentMode = VK_PRESENT_MODE_FIFO_KHR;

        m_SwapchainData.Images.resize(m_SwapchainData.ImageCount);
        m_SwapchainData.ImageViews.resize(m_SwapchainData.ImageCount);
        m_SwapchainData.Framebuffers.resize(m_SwapchainData.ImageCount);

        // Create Swapchain
        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = m_SwapchainData.ImageCount;
        swapchainInfo.imageFormat = m_SurfaceFormat.format;
        swapchainInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        swapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.presentMode = m_SwapchainData.PresentMode;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: look into this variable
        // TODO: other settings

        result = vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain);
        CheckVkResult(result);

        if (m_Swapchain)
        {
            Logger::LogInfo("VkSwapchain info:");
            Logger::LogInfo("- Image count: " + std::to_string(m_SwapchainData.ImageCount));
            Logger::LogInfo("- Present mode: " + std::string(string_VkPresentModeKHR(m_SwapchainData.PresentMode)));
        }

        // Get the images from the swapchain
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);

        if (swapchainImageCount != m_SwapchainData.ImageCount)
        {
            Logger::LogError("Swapchain created a different number of images than the number specified");
            return false;
        }

        result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, m_SwapchainData.Images.data());
        CheckVkResult(result);

        // Create Image Views for swapchain images
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = m_SwapchainData.Images[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_SurfaceFormat.format;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // subresourceRange determines how the image should be accessed
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            result = vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapchainData.ImageViews[i]);
            CheckVkResult(result);
        }

        return true;
    }

    bool VulkanContext::CreateFramebuffers(const VkRenderPass& renderPass)
    {
        for (uint32_t i = 0; i < m_SwapchainData.ImageCount; i++)
        {
            VkImageView attachments[] = {
                m_SwapchainData.ImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = 640; // TODO: width and height should be swapchain extent
            framebufferInfo.height = 480;
            framebufferInfo.layers = 1;

            auto result = vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapchainData.Framebuffers[i]);
            CheckVkResult(result);

            if (result != VK_SUCCESS)
            {
                return false;
            }
        }

        return true;
    }

    bool VulkanContext::CreateSyncObjects()
    {
        VkResult result;

        // ---------------------
        // Create command pool
        // ---------------------

        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex.value();

        result = vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_CommandPool);
        CheckVkResult(result);

        // ---------------------
        // Create command buffers
        // ---------------------

        VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
        commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocInfo.commandPool = m_CommandPool;
        commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocInfo.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(m_Device, &commandBufferAllocInfo, &m_CommandBuffer);
        CheckVkResult(result);

        // ---------------------
        // Create synchronization objects
        // ---------------------

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // We create it signalled so the first frame doesn't wait for an unsignallable fence

        result = vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
        CheckVkResult(result);

        result = vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
        CheckVkResult(result);

        result = vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFence);
        CheckVkResult(result);

        return true;
    }

    void VulkanContext::RecordCommands(uint32_t imageIndex)
    {
        VkResult result;

        // ---------------------
        // Record commands into buffer
        // ---------------------

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0; // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional // Used in secondary command buffers

        //result = vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo);
        //CheckVkResult(result);
        vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo);

        // ---------------------
        // Begin Render Pass
        // ---------------------
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_Renderpass->GetVKRenderPass();
        renderPassBeginInfo.framebuffer = m_SwapchainData.Framebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = { 640, 480 }; // TODO: THIS SHOULD BE SWAPCHAIN EXTENT
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &m_ClearColour;

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // can't do error checking

        // ---------------------
        // Drawing
        // ---------------------

        // Bind the graphics pipeline
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetVKPipeline());
        

        auto viewport = m_Pipeline->GetViewport();
        auto scissor = m_Pipeline->GetScissor();

        // Set dynamic state objects
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

        // Draw
        vkCmdDraw(m_CommandBuffer, 0, 1, 0, 0); // instance count must be 1 if we aren't doing any instancing

        // End render pass
        vkCmdEndRenderPass(m_CommandBuffer);

        // Finish recording the command buffer
        //result = vkEndCommandBuffer(m_CommandBuffer);
        //CheckVkResult(result);
        vkEndCommandBuffer(m_CommandBuffer);
    }

    const std::vector<VkLayerProperties> VulkanContext::GetAvailableLayers()
    {
        // Get layer count
        uint32_t availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

        // Get layers
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        return availableLayers;
    }

    const std::vector<const char*> VulkanContext::GetValidationLayers(const std::vector<VkLayerProperties>& availableLayers)
    {
        const char* validationLayers[] = {
            "VK_LAYER_KHRONOS_validation" // TODO: implement a system that selects the correct validation layers in a priority
        };
        uint32_t enabledLayerCount = 1;

        std::vector<const char*> selectedValidationLayers;
            
        // Find enabled layers and throw errors if any are not found
        for (const auto& layer : validationLayers) // idk if this should be const auto&
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layer, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                }
            }

            if (layerFound)
            {
                selectedValidationLayers.push_back(layer);
            }
        }

        return selectedValidationLayers;
    }

    const std::vector<VkPhysicalDevice> VulkanContext::GetAvailablePhysicalDevices()
    {
        // Get available physical device count
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            Logger::LogError("Failed to find any vulkan supported GPU");
            return std::vector<VkPhysicalDevice>();
        }

        // Get Vulkan supported physical devices (GPUs)
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        return devices;
    }

    const std::vector<VkQueueFamilyProperties> VulkanContext::GetQueueFamilies(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        if (queueFamilyCount < 1)
        {
            Logger::LogError("Physical device has no queue families :(");
            return std::vector<VkQueueFamilyProperties>();
        }

        Logger::LogInfo("Found " + std::to_string(queueFamilyCount) + " queue families on physical device");

        // Retrieve queue families
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        // for (uint32_t i = 0; i < queueFamilyCount; i++)
        // {
        //     Logger::LogInfo("Queue Family " + std::to_string(i + 1) + ":");
        //     Logger::LogInfo(" - " + string_VkQueueFlags(queueFamilies[i].queueFlags));
        // }

        return queueFamilies;
    }

    const std::vector<VkExtensionProperties> VulkanContext::GetDeviceExtensions(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

        return extensions;
    }

    const std::vector<VkSurfaceFormatKHR> VulkanContext::GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

        return surfaceFormats;
    }


    const std::vector<VkPresentModeKHR> VulkanContext::GetSurfacePresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {   
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

        return presentModes;
    }

    const VkSurfaceCapabilitiesKHR VulkanContext::GetSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {   
        VkSurfaceCapabilitiesKHR capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        
        return capabilities;
    }

    bool VulkanContext::SelectFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices)
    {
        // Find a suitable discrete gpu physical device
        bool foundSuitableDevice = false;
        for (const auto& device : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_GPU = device;
                foundSuitableDevice = true;
                Logger::LogInfo("Selected discrete GPU: " + std::string((const char*)properties.deviceName));
                Logger::LogInfo("- Driver Version: " + std::to_string(properties.driverVersion));
                Logger::LogInfo("- Supported Vulkan API Version: " + std::to_string(properties.apiVersion));
                break;
            }
        }

        if (foundSuitableDevice)
            return true;

        return false;
    }

    bool VulkanContext::SelectFirstVKCapableGPU(const std::vector<VkPhysicalDevice>& physicalDevices)
    {
        m_GPU = physicalDevices[0];

        if (m_GPU = VK_NULL_HANDLE) // no scenario should ever find this null pointer
            return false;

        // Log selected GPU
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[0], &properties);

        Logger::LogInfo("Selected non-discrete VK capable GPU: " + std::to_string((const char)properties.deviceName));
        return true;
    }

    uint32_t VulkanContext::GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies)
    {
        bool foundGraphicsQueue = false;
        uint32_t graphicsQueueIndex = -1;

        // Find suitable queue families
        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            // Find suitable graphics queue family
            if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && !foundGraphicsQueue)
            {
                // Check if this graphics queue family supports presentation to a surface
                VkBool32 surfaceSupport;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_GPU, i, m_Surface, &surfaceSupport);

                if (surfaceSupport == VK_TRUE)
                {
                    graphicsQueueIndex = i;
                    foundGraphicsQueue = true;
                    break;
                }
            }
        }

        if (!foundGraphicsQueue)
            Logger::LogError("Failed to find a suitable graphics queue from physical device");
        else
            Logger::LogInfo("Successfully retrieved graphics queue from physical device");

        return graphicsQueueIndex;
        
    }

}