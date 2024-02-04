#include "VulkanSwapchain.h"

#include "VulkanHelpers.h"
#include "vulkan/vk_enum_string_helper.h"

namespace pxl
{
    VulkanSwapchain::VulkanSwapchain(VkDevice device, VkPhysicalDevice gpu, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, const std::shared_ptr<Window> windowHandle, const std::shared_ptr<VulkanRenderPass>& renderPass)
        : m_Device(device), m_GPU(gpu), m_Surface(surface), m_WindowHandle(windowHandle), m_DefaultRenderPass(renderPass)
    {
        m_SwapchainSpecs.Format = surfaceFormat.format;
        m_SwapchainSpecs.ColorSpace = surfaceFormat.colorSpace;

        Create();
        PrepareImages();
        PrepareFramebuffers(renderPass);
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        Destroy();
    }

    void VulkanSwapchain::Create()
    {
        m_SwapchainSpecs.PresentMode = GetSuitablePresentMode();
        m_SwapchainSpecs.ImageCount = GetSuitableImageCount();

        // Check if extent wasnt custom set
        if (m_SwapchainSpecs.Extent.width == UINT32_MAX)
        {
            // Set extent manually
            auto fbSize = m_WindowHandle->GetFramebufferSize();
            m_SwapchainSpecs.Extent = { fbSize.x, fbSize.y };
            PXL_LOG_WARN(LogArea::Vulkan, "Manually set swapchain extent to window framebuffer size"); // TODO: temporary
        }

        CheckExtentSupport(m_SwapchainSpecs.Extent);
        
        // Create Swapchain
        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = m_SwapchainSpecs.ImageCount;
        swapchainInfo.imageFormat = m_SwapchainSpecs.Format;
        swapchainInfo.imageColorSpace = m_SwapchainSpecs.ColorSpace;
        swapchainInfo.imageExtent = m_SwapchainSpecs.Extent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // The image will be used as a colour attachment
        swapchainInfo.presentMode = m_SwapchainSpecs.PresentMode;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: look into this variable
        // TODO: other settings

        VkResult result = vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain);
        VulkanHelpers::CheckVkResult(result);

        if (m_Swapchain)
        {
            Logger::LogInfo("Swapchain created:");
            Logger::LogInfo("- Present mode: " + std::string(string_VkPresentModeKHR(m_SwapchainSpecs.PresentMode)));
            Logger::LogInfo("- Image count: " + std::to_string(m_SwapchainSpecs.ImageCount));
            Logger::LogInfo("- Image format: " + std::string(string_VkFormat(m_SwapchainSpecs.Format)));
            Logger::LogInfo("- Image color space: " + std::string(string_VkColorSpaceKHR(m_SwapchainSpecs.ColorSpace)));
            Logger::LogInfo("- Image extent: " + std::to_string(m_SwapchainSpecs.Extent.width) + "x" + std::to_string(m_SwapchainSpecs.Extent.height));
        }
        else
        {
            Logger::LogError("Failed to create swapchain");
        }
    }

    void VulkanSwapchain::Recreate()
    {
        vkDeviceWaitIdle(m_Device);

        Destroy(); // cleanup swapchain

        // Get framebuffer size for new swapchain
        auto fbSize = m_WindowHandle->GetFramebufferSize();
        m_SwapchainSpecs.Extent = { fbSize.x, fbSize.y };

        // Create new swapchain
        Create();
        PrepareImages();
        PrepareFramebuffers(m_DefaultRenderPass);
    }

    void VulkanSwapchain::Destroy()
    {
        vkDeviceWaitIdle(m_Device);

        for (auto& framebuffer : m_Framebuffers)
            framebuffer->Destroy();

        for (auto& image : m_Images)
            image->Destroy();

        if (m_Swapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
    }

    uint32_t VulkanSwapchain::AcquireNextAvailableImageIndex(VkSemaphore signalSemaphore)
    { 
        uint32_t imageIndex;
        
        auto result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate();
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            Logger::LogError("Failed to retrieve next available image in the swapchain");
        }

        return imageIndex;
    }

    void VulkanSwapchain::QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore)
    {
        VkResult result;

        VkSwapchainKHR swapChains[] = { m_Swapchain };
        VkSemaphore waitSemaphores[] = { waitSemaphore }; // wait for the command buffers to finish executing (rendering) to finish before presenting
        
        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphores;
        presentInfo.pResults = nullptr; // useful for error checking when using multiple swap chains
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        // Queue presentation
        result = vkQueuePresentKHR(queue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
        {
            Recreate();
        } 
        else if (result != VK_SUCCESS) 
        {
            Logger::LogError("Failed to queue image presentation");
        }
    }

    void VulkanSwapchain::PrepareImages()
    {
        m_Images.resize(m_SwapchainSpecs.ImageCount);
        
        // Get swapchain image count
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);

        if (swapchainImageCount != m_SwapchainSpecs.ImageCount)
        {
            Logger::LogError("Swapchain created a different number of images than the number specified");
            return;
        }

        std::vector<VkImage> images(swapchainImageCount);

        // Get swapchain images
        auto result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, images.data());
        VulkanHelpers::CheckVkResult(result);

        // Create image view for each swapchain image
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            std::shared_ptr<VulkanImage> image;
            image = std::make_shared<VulkanImage>(m_Device, m_SwapchainSpecs.Extent.width, m_SwapchainSpecs.Extent.height, m_SwapchainSpecs.Format, images[i]);
            m_Images[i] = image;
        }
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
        auto availablePresentModes = VulkanHelpers::GetSurfacePresentModes(m_GPU, m_Surface);

        VkPresentModeKHR suitablePresentMode;
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
            Logger::LogError("Failed to find suitable swap chain present mode, defaulting to VK_PRESENT_MODE_FIFO_KHR");
            suitablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        return suitablePresentMode;
    }

    uint32_t VulkanSwapchain::GetSuitableImageCount()
    {
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(m_GPU, m_Surface);

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
            Logger::LogError("Selected surface for swapchain must support more than 2 images"); // technically its guaranteed to support 1 image but a game will always need 2 I think.
        }

        return suitableImageCount;
    }

    bool VulkanSwapchain::CheckExtentSupport(VkExtent2D extent)
    {
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(m_GPU, m_Surface);

        if ((extent.width >= surfaceCapabilities.minImageExtent.width && extent.height >= surfaceCapabilities.minImageExtent.height)
            && (extent.width <= surfaceCapabilities.maxImageExtent.width && extent.height <= surfaceCapabilities.maxImageExtent.height))
        {
            return true;
        }
        else
        {
            Logger::LogError("Specified swapchain extent isnt supported by the surface");
        }

        return false;
    }
}