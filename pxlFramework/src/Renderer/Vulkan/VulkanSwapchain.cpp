#include "VulkanSwapchain.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanSwapchain::VulkanSwapchain(const std::shared_ptr<VulkanDevice>& device, VkSurfaceKHR surface, const VkSurfaceFormatKHR& surfaceFormat, const VkExtent2D& imageExtent, const std::shared_ptr<VulkanRenderPass>& renderPass)
        : m_Device(device), m_Surface(surface), m_DefaultRenderPass(renderPass)
    {
        m_SwapchainSpecs.Extent = imageExtent;
        m_SwapchainSpecs.Format = surfaceFormat.format;
        m_SwapchainSpecs.ColorSpace = surfaceFormat.colorSpace;
        m_SwapchainSpecs.PresentMode = GetSuitablePresentMode();
        m_SwapchainSpecs.ImageCount = GetSuitableImageCount();

        CreateSwapchain();

        PrepareImages();
        PrepareFramebuffers(m_DefaultRenderPass);

        // Prepare swapchain frames
        m_Frames.resize(m_MaxFramesInFlight);

        auto vkDevice = static_cast<VkDevice>(m_Device->GetLogical());
        auto commandBuffers = m_Device->AllocateCommandBuffers(QueueType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, static_cast<uint32_t>(m_Frames.size()));

        for (size_t i = 0; i < m_Frames.size(); i++)
        {
            VulkanFrame frame;
            frame.CommandBuffer = commandBuffers[i];
            frame.ImageAvailableSemaphore = VulkanHelpers::CreateSemaphore(vkDevice);
            frame.RenderFinishedSemaphore = VulkanHelpers::CreateSemaphore(vkDevice);
            frame.InFlightFence = VulkanHelpers::CreateFence(vkDevice, true);
            m_Frames[i] = frame;
        }

        VulkanDeletionQueue::Add([&]()
        {
            DestroyFrameData();
            Destroy(m_Swapchain);
        });
    }

    void VulkanSwapchain::CreateSwapchain()
    {
        m_SwapchainSpecs.PresentMode = m_VSync ? VK_PRESENT_MODE_FIFO_KHR : GetSuitablePresentMode();
        m_SwapchainSpecs.ImageCount = GetSuitableImageCount();

        CheckExtentSupport(m_SwapchainSpecs.Extent);

        auto oldSwapchain = m_Swapchain;

        // Create Swapchain
        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.pNext = nullptr;
        swapchainInfo.flags = 0;
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = m_SwapchainSpecs.ImageCount; // NOTE: May create an amount of images higher than this specified number
        swapchainInfo.imageFormat = m_SwapchainSpecs.Format;
        swapchainInfo.imageColorSpace = m_SwapchainSpecs.ColorSpace;
        swapchainInfo.imageExtent = m_SwapchainSpecs.Extent;
        swapchainInfo.imageArrayLayers = 1;                             // NOTE (from vk spec): For non-stereoscopic-3D applications, this value is 1.
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // The image will be used as a colour attachment. It wouldnt make sense to use anything else here really
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;     // Exclusive = 1 queue family will access the images, Concurrent = multiple queue families will access the images
        swapchainInfo.queueFamilyIndexCount = 0;                        // Only required when imageSharingMode is concurrent
        swapchainInfo.pQueueFamilyIndices = VK_NULL_HANDLE;             // Only required when imageSharingMode is concurrent
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Specifies how the operating system will use the surfaces alpha value
        swapchainInfo.presentMode = m_SwapchainSpecs.PresentMode;
        swapchainInfo.clipped = VK_TRUE; // NOTE: may cause issues with fragment shaders when enabled
        swapchainInfo.oldSwapchain = oldSwapchain;

        VK_CHECK(vkCreateSwapchainKHR(static_cast<VkDevice>(m_Device->GetLogical()), &swapchainInfo, nullptr, &m_Swapchain));

        if (oldSwapchain)
            Destroy(oldSwapchain);

        // Logging
        if (m_Swapchain)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "Vulkan swapchain created:");
            PXL_LOG_INFO(LogArea::Vulkan, "- Present mode: {}", string_VkPresentModeKHR(m_SwapchainSpecs.PresentMode));
            PXL_LOG_INFO(LogArea::Vulkan, "- Image count: {}", m_SwapchainSpecs.ImageCount);
            PXL_LOG_INFO(LogArea::Vulkan, "- Image format: {}", string_VkFormat(m_SwapchainSpecs.Format));
            PXL_LOG_INFO(LogArea::Vulkan, "- Image color space: {}", string_VkColorSpaceKHR(m_SwapchainSpecs.ColorSpace));
            PXL_LOG_INFO(LogArea::Vulkan, "- Image extent: {}x{}", m_SwapchainSpecs.Extent.width, m_SwapchainSpecs.Extent.height);
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create swapchain");
        }
    }

    void VulkanSwapchain::Recreate(uint32_t width, uint32_t height)
    {
        m_SwapchainSpecs.Extent = { width, height };
        Recreate();
    }

    void VulkanSwapchain::Recreate()
    {
        PXL_PROFILE_SCOPE;

        m_Device->WaitIdle();

        CreateSwapchain();
        PrepareImages();
        PrepareFramebuffers(m_DefaultRenderPass);
    }

    void VulkanSwapchain::Destroy(VkSwapchainKHR swapchain)
    {
        for (auto& framebuffer : m_Framebuffers)
            framebuffer->Destroy();

        for (auto& image : m_Images)
            image->Destroy();

        if (swapchain)
            vkDestroySwapchainKHR(static_cast<VkDevice>(m_Device->GetLogical()), swapchain, nullptr);

        if (swapchain == m_Swapchain)
            m_Swapchain = VK_NULL_HANDLE;
    }

    void VulkanSwapchain::DestroyFrameData()
    {
        auto device = static_cast<VkDevice>(m_Device->GetLogical());

        for (auto& frame : m_Frames)
        {
            if (frame.ImageAvailableSemaphore)
            {
                vkDestroySemaphore(device, frame.ImageAvailableSemaphore, nullptr);
                frame.ImageAvailableSemaphore = VK_NULL_HANDLE;
            }

            if (frame.RenderFinishedSemaphore)
            {
                vkDestroySemaphore(device, frame.RenderFinishedSemaphore, nullptr);
                frame.RenderFinishedSemaphore = VK_NULL_HANDLE;
            }

            if (frame.InFlightFence)
            {
                vkDestroyFence(device, frame.InFlightFence, nullptr);
                frame.InFlightFence = VK_NULL_HANDLE;
            }
        }
    }

    void VulkanSwapchain::AcquireNextAvailableImageIndex()
    {
        if (m_Suspend)
            return;

        auto result = vkAcquireNextImageKHR(static_cast<VkDevice>(m_Device->GetLogical()), m_Swapchain, UINT64_MAX, m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            Recreate();
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to retrieve next available image in the swapchain");
    }

    void VulkanSwapchain::QueuePresent()
    {
        PXL_PROFILE_SCOPE;

        if (m_Suspend)
            return;

        VkSwapchainKHR swapChains[] = { m_Swapchain };
        VkSemaphore waitSemaphores[] = { m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore }; // wait for the command buffers to finish executing (rendering) to finish before presenting

        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphores;
        presentInfo.pResults = nullptr; // useful for error checking when using multiple swap chains
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_CurrentImageIndex;

        // Queue presentation
        auto result = m_Device->SubmitPresent(presentInfo);

        // Update current frame index to the next frame
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
        {
            m_FramebufferResized = false;
            Recreate();
        }
        else if (result != VK_SUCCESS)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to queue image presentation");
        }
    }

    void VulkanSwapchain::PrepareImages()
    {
        m_Images.resize(m_SwapchainSpecs.ImageCount);

        auto device = static_cast<VkDevice>(m_Device->GetLogical());

        // Get swapchain image count
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(device, m_Swapchain, &swapchainImageCount, nullptr);

        if (swapchainImageCount != m_SwapchainSpecs.ImageCount)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Swapchain created a different number of images than the number specified");
            return;
        }

        std::vector<VkImage> images(swapchainImageCount);

        // Get swapchain images
        VK_CHECK(vkGetSwapchainImagesKHR(device, m_Swapchain, &swapchainImageCount, images.data()));

        // Create an imageless image object containing an image view for each swapchain image
        for (uint32_t i = 0; i < swapchainImageCount; i++)
            m_Images[i] = std::make_shared<VulkanImage>(m_Device, m_SwapchainSpecs.Extent.width, m_SwapchainSpecs.Extent.height, m_SwapchainSpecs.Format, images[i]);
    }

    void VulkanSwapchain::PrepareFramebuffers(const std::shared_ptr<VulkanRenderPass>& renderPass)
    {
        m_Framebuffers.resize(m_SwapchainSpecs.ImageCount);

        // Create swapchain framebuffers
        for (uint32_t i = 0; i < m_SwapchainSpecs.ImageCount; i++)
        {
            std::shared_ptr<VulkanFramebuffer> framebuffer;
            framebuffer = std::make_shared<VulkanFramebuffer>(m_Device, renderPass, m_SwapchainSpecs.Extent);
            framebuffer->AddAttachment(m_Images[i]->GetImageView(), m_SwapchainSpecs.Format);
            framebuffer->Recreate();
            m_Framebuffers[i] = framebuffer;
        }
    }

    VkPresentModeKHR VulkanSwapchain::GetSuitablePresentMode()
    {
        auto availablePresentModes = VulkanHelpers::GetSurfacePresentModes(static_cast<VkPhysicalDevice>(m_Device->GetPhysical()), m_Surface);

        VkPresentModeKHR suitablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
        bool foundSuitablePresentMode = false;

        for (const auto& presentMode : availablePresentModes)
        {
            // Mailbox present mode is low latency Vsync, we prefer it if available
            if (m_VSync && presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                suitablePresentMode = presentMode;
                foundSuitablePresentMode = true;
                break;
            }

            if (!m_VSync && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                suitablePresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                foundSuitablePresentMode = true;
                break;
            }
        }

        if (!foundSuitablePresentMode)
        {
            PXL_LOG_WARN(LogArea::Vulkan, "Failed to find suitable swap chain present mode, defaulting to VK_PRESENT_MODE_FIFO_KHR");
            suitablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        return suitablePresentMode;
    }

    uint32_t VulkanSwapchain::GetSuitableImageCount()
    {
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(static_cast<VkPhysicalDevice>(m_Device->GetPhysical()), m_Surface);

        uint32_t suitableImageCount = 0;

        // Select most suitable number of images for swapchain
        if (surfaceCapabilities.maxImageCount >= 2)
        {
            suitableImageCount = surfaceCapabilities.maxImageCount >= 3 ? 3 : 2; // Triple/Double buffering
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Selected surface for swapchain must support 2 or more images"); // technically its guaranteed to support 1 image but a game will always need 2 I think.
        }

        return suitableImageCount;
    }

    bool VulkanSwapchain::CheckExtentSupport(VkExtent2D extent)
    {
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(static_cast<VkPhysicalDevice>(m_Device->GetPhysical()), m_Surface);

        if ((extent.width >= surfaceCapabilities.minImageExtent.width && extent.height >= surfaceCapabilities.minImageExtent.height) && (extent.width <= surfaceCapabilities.maxImageExtent.width && extent.height <= surfaceCapabilities.maxImageExtent.height))
            return true;
        else
            PXL_LOG_ERROR(LogArea::Vulkan, "Specified swapchain extent isnt supported by the surface");

        return false;
    }
}