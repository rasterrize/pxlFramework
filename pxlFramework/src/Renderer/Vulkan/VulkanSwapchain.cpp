#include "VulkanSwapchain.h"

#include "VulkanHelpers.h"
#include "VulkanContext.h"

namespace pxl
{
    VulkanSwapchain::VulkanSwapchain(const std::shared_ptr<VulkanDevice>& device, VkSurfaceKHR surface, const VkSurfaceFormatKHR& surfaceFormat, const VkExtent2D& imageExtent, const std::shared_ptr<VulkanRenderPass>& renderPass, VkCommandPool commandPool)
        : m_Device(device), m_Surface(surface), m_DefaultRenderPass(renderPass)
    {
        m_SwapchainSpecs.Extent = imageExtent;
        m_SwapchainSpecs.Format = surfaceFormat.format;
        m_SwapchainSpecs.ColorSpace = surfaceFormat.colorSpace;
        m_SwapchainSpecs.PresentMode = GetSuitablePresentMode();
        m_SwapchainSpecs.ImageCount = GetSuitableImageCount();

        Create();

        // Prepare swapchain frames
        m_Frames.resize(m_MaxFramesInFlight);

        // Get command pool from graphics context
        auto commandBuffers = VulkanHelpers::AllocateCommandBuffers(device->GetVkDevice(), commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, static_cast<uint32_t>(m_Frames.size()));
        uint32_t index = 0;

        for (auto& frame : m_Frames)
        {
            frame.CommandBuffer = commandBuffers[index];
            frame.ImageAvailableSemaphore = VulkanHelpers::CreateSemaphore(device->GetVkDevice());
            frame.RenderFinishedSemaphore = VulkanHelpers::CreateSemaphore(device->GetVkDevice());
            frame.InFlightFence = VulkanHelpers::CreateFence(device->GetVkDevice(), true);

            index++;
        }

        PrepareImages();
        PrepareFramebuffers(renderPass);
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        DestroyFrameData();
        Destroy();
    }

    void VulkanSwapchain::Create()
    {
        // m_SwapchainSpecs.PresentMode = GetSuitablePresentMode();
        // m_SwapchainSpecs.ImageCount = GetSuitableImageCount();

        // Check extent support
        // if (m_SwapchainSpecs.Extent.width == 0 || m_SwapchainSpecs.Extent.height == 0)
        //     return;

        CheckExtentSupport(m_SwapchainSpecs.Extent);
        
        // Create Swapchain
        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = m_SwapchainSpecs.ImageCount; // creates an amount of images higher than this specified number, we should probably check to make sure the vkCreateSwapchainKHR doesn't create any extra images
        swapchainInfo.imageFormat = m_SwapchainSpecs.Format;
        swapchainInfo.imageColorSpace = m_SwapchainSpecs.ColorSpace;
        swapchainInfo.imageExtent = m_SwapchainSpecs.Extent;
        swapchainInfo.imageArrayLayers = 1; // NOTE (from vk spec): For non-stereoscopic-3D applications, this value is 1.
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // The image will be used as a colour attachment. It wouldnt make sense to use anything else here really
        swapchainInfo.presentMode = m_SwapchainSpecs.PresentMode;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Specifies how the operating system will use the surfaces alpha value
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Exclusive = 1 queue family will access the images, Concurrent = multiple queue families will access the images
        // TODO: other settings

        // bool usingOldSwapchain = false;
        // if (m_Swapchain != VK_NULL_HANDLE)
        // {
        //     swapchainInfo.oldSwapchain = m_Swapchain;
        //     usingOldSwapchain = true;
        // }

        //VkSwapchainKHR newSwapchain;
        VK_CHECK(vkCreateSwapchainKHR(m_Device->GetVkDevice(), &swapchainInfo, nullptr, &m_Swapchain));

        // if (usingOldSwapchain)
        //     vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_Swapchain, nullptr); // destroy the old swapchain if it was used for recreation

        //m_Swapchain = newSwapchain;

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

    void VulkanSwapchain::Recreate()
    {
        m_Device->WaitIdle();

        Destroy();

        // Swapchain image extent doesn't change, but other variables such as vertical sync may have been set so we recreate anyway
        

        // Create new swapchain
        Create();
        PrepareImages();
        PrepareFramebuffers(m_DefaultRenderPass);
    }

    // void VulkanSwapchain::Recreate(uint32_t width, uint32_t height)
    // {
    //     // if (width == 0 || height == 0)
    //     //     return;

    //     m_Device->WaitIdle();

    //     Destroy();

    //     // Set new swapchain image extent
    //     m_SwapchainSpecs.Extent = { width, height };

    //     // Create new swapchain
    //     Create();
    //     PrepareImages();
    //     PrepareFramebuffers(m_DefaultRenderPass);
    // }

    void VulkanSwapchain::Destroy()
    {
        m_Device->WaitIdle();

        for (auto& framebuffer : m_Framebuffers)
            framebuffer->Destroy();

        for (auto& image : m_Images)
            image->Destroy();

        if (m_Swapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_Swapchain, nullptr);
    }

    void VulkanSwapchain::DestroyFrameData()
    {
        m_Device->WaitIdle();

        for (auto& frame : m_Frames)
        {
            if (frame.ImageAvailableSemaphore != VK_NULL_HANDLE)
                vkDestroySemaphore(m_Device->GetVkDevice(), frame.ImageAvailableSemaphore, nullptr);

            if (frame.RenderFinishedSemaphore != VK_NULL_HANDLE)
                vkDestroySemaphore(m_Device->GetVkDevice(), frame.RenderFinishedSemaphore, nullptr);
        }
    }

    void VulkanSwapchain::AcquireNextAvailableImageIndex()
    { 
        auto result = vkAcquireNextImageKHR(m_Device->GetVkDevice(), m_Swapchain, UINT64_MAX, m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate();
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to retrieve next available image in the swapchain");
        }
    }

    void VulkanSwapchain::QueuePresent(VkQueue queue)
    {
        // Dont queue frames if the framebuffer is 0, 0 (aka the window is minimized)
        // if (m_SwapchainSpecs.Extent.width == 0 || m_SwapchainSpecs.Extent.height == 0)
        //     return;

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
        VkResult result = vkQueuePresentKHR(queue, &presentInfo);
        
        // Update current frame index to the next frame
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
        {
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
        
        // Get swapchain image count
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_Swapchain, &swapchainImageCount, nullptr);

        if (swapchainImageCount != m_SwapchainSpecs.ImageCount)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Swapchain created a different number of images than the number specified");
            return;
        }

        std::vector<VkImage> images(swapchainImageCount);

        // Get swapchain images
        VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_Swapchain, &swapchainImageCount, images.data()));

        // Create an imageless image object containing an image view for each swapchain image
        for (uint32_t i = 0; i < swapchainImageCount; i++)
            m_Images[i] = std::make_shared<VulkanImage>(m_Device->GetVkDevice(), m_SwapchainSpecs.Extent.width, m_SwapchainSpecs.Extent.height, m_SwapchainSpecs.Format, images[i]);
    }

    void VulkanSwapchain::PrepareFramebuffers(const std::shared_ptr<VulkanRenderPass>& renderPass)
    {
        m_Framebuffers.resize(m_SwapchainSpecs.ImageCount);

        // Create swapchain framebuffers
        for (uint32_t i = 0; i < m_SwapchainSpecs.ImageCount; i++)
        {
            std::shared_ptr<VulkanFramebuffer> framebuffer;
            framebuffer = std::make_shared<VulkanFramebuffer>(m_Device->GetVkDevice(), renderPass, m_SwapchainSpecs.Extent);
            framebuffer->AddAttachment(m_Images[i]->GetImageView(), m_SwapchainSpecs.Format);
            framebuffer->Recreate();
            m_Framebuffers[i] = framebuffer;
        }
    }

    VkPresentModeKHR VulkanSwapchain::GetSuitablePresentMode()
    {
        auto availablePresentModes = VulkanHelpers::GetSurfacePresentModes(m_Device->GetVkPhysicalDevice(), m_Surface);

        VkPresentModeKHR suitablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
        bool foundSuitablePresentMode = false;

        // If vsync is enabled we dont need to do any checking since FIFO should already be supported
        if (m_VSync)
        {
            suitablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
            foundSuitablePresentMode = true;
        }
        else
        {
            for (const auto& presentMode : availablePresentModes)
            {
                // Mailbox present mode is the most ideal
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    suitablePresentMode = presentMode;
                    foundSuitablePresentMode = true;
                    break;
                }

                // Use immediate present mode but still loop through incase mailbox is still supported
                if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			    {
				    suitablePresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                    foundSuitablePresentMode = true;
			    }
            }
        }
        
        if (!foundSuitablePresentMode)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to find suitable swap chain present mode, defaulting to VK_PRESENT_MODE_FIFO_KHR");
            suitablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        return suitablePresentMode;
    }

    uint32_t VulkanSwapchain::GetSuitableImageCount()
    {
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(m_Device->GetVkPhysicalDevice(), m_Surface);

        uint32_t suitableImageCount = 0;

        // Select most suitable number of images for swapchain
        if (surfaceCapabilities.minImageCount >= 2)
        {
            if (surfaceCapabilities.maxImageCount >= 3)
            {
                suitableImageCount = 3; // Triple buffering
            }
            else
            {
                suitableImageCount = 2; // Double buffering
            }
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Selected surface for swapchain must support more than 2 images"); // technically its guaranteed to support 1 image but a game will always need 2 I think.
        }

        return suitableImageCount;
    }

    bool VulkanSwapchain::CheckExtentSupport(VkExtent2D extent)
    {
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(m_Device->GetVkPhysicalDevice(), m_Surface);

        if ((extent.width >= surfaceCapabilities.minImageExtent.width && extent.height >= surfaceCapabilities.minImageExtent.height)
            && (extent.width <= surfaceCapabilities.maxImageExtent.width && extent.height <= surfaceCapabilities.maxImageExtent.height))
        {
            return true;
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Specified swapchain extent isnt supported by the surface");
        }

        return false;
    }
}