#include "VulkanSwapchain.h"

#include "VulkanHelpers.h"
#include "vulkan/vk_enum_string_helper.h"

namespace pxl
{
    VulkanSwapchain::VulkanSwapchain(VkDevice device, VkSurfaceKHR surface, const VulkanSwapchainData& swapchainData, const std::shared_ptr<VulkanRenderPass>& renderPass)
        : m_Device(device), m_Surface(surface), m_Data(swapchainData)
    {
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
        // Create Swapchain
        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = m_Data.ImageCount;
        swapchainInfo.imageFormat = m_Data.Format;
        swapchainInfo.imageColorSpace = m_Data.ColorSpace;
        swapchainInfo.imageExtent = m_Data.Extent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // The image will be used as a colour attachment
        swapchainInfo.presentMode = m_Data.PresentMode;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: look into this variable
        // TODO: other settings

        VkResult result = vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain);
        VulkanHelpers::CheckVkResult(result);

        if (m_Swapchain)
        {
            Logger::LogInfo("Swapchain created:");
            Logger::LogInfo("- Present mode: " + std::string(string_VkPresentModeKHR(m_Data.PresentMode)));
            Logger::LogInfo("- Image count: " + std::to_string(m_Data.ImageCount));
            Logger::LogInfo("- Image format: " + std::string(string_VkFormat(m_Data.Format)));
            Logger::LogInfo("- Image color space: " + std::string(string_VkColorSpaceKHR(m_Data.ColorSpace)));
            Logger::LogInfo("- Image extent: " + std::to_string(m_Data.Extent.width) + "x" + std::to_string(m_Data.Extent.height));


        }
    }

    void VulkanSwapchain::Recreate()
    {
        vkDeviceWaitIdle(m_Device);


    }

    void VulkanSwapchain::Destroy()
    {
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
        
        if (vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
        {
            Logger::LogError("Failed to retrieve next available image in the swapchain");
            return -1;
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
        VulkanHelpers::VulkanHelpers::CheckVkResult(result);
    }

    void VulkanSwapchain::PrepareImages()
    {
        m_Images.resize(m_Data.ImageCount);
        
        // Get swapchain image count
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);

        if (swapchainImageCount != m_Data.ImageCount)
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
            image = std::make_shared<VulkanImage>(m_Device, m_Data.Extent.width, m_Data.Extent.height, m_Data.Format, images[i]);
            m_Images[i] = image;
        }
    }

    void VulkanSwapchain::PrepareFramebuffers(const std::shared_ptr<VulkanRenderPass>& renderPass)
    {
        m_Framebuffers.resize(m_Data.ImageCount);

        // Create swapchain framebuffers
        for (uint32_t i = 0; i < m_Data.ImageCount; i++)
        {
            std::shared_ptr<VulkanFramebuffer> framebuffer;
            framebuffer = std::make_shared<VulkanFramebuffer>(m_Device, renderPass, m_Data.Extent);
            framebuffer->AddAttachment(m_Images[i]->GetImageView(), m_Data.Format);
            framebuffer->Recreate();
            m_Framebuffers[i] = framebuffer;
        }
    }
}