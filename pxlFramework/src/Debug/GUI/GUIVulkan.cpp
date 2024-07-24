#include "GUIVulkan.h"

#include <backends/imgui_impl_vulkan.h>

#include "Renderer/Vulkan/VulkanHelpers.h"
#include "Renderer/Vulkan/VulkanInstance.h"

namespace pxl
{
    GUIVulkan::GUIVulkan(const std::shared_ptr<GraphicsContext>& context)
        : m_ContextHandle(static_pointer_cast<VulkanGraphicsContext>(context))
    {
        m_Device = std::static_pointer_cast<VulkanDevice>(context->GetDevice());

        // ImGui descriptor pool
        VkDescriptorPoolSize pool_sizes[] =
	    {
	    	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
	    	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	    };

	    VkDescriptorPoolCreateInfo pool_info = {};
	    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	    pool_info.maxSets = 1000;
	    pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
	    pool_info.pPoolSizes = pool_sizes;

	    VK_CHECK(vkCreateDescriptorPool(static_cast<VkDevice>(m_Device->GetDevice()), &pool_info, nullptr, &m_ImGuiDescriptorPool));

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = VulkanInstance::Get();
        initInfo.PhysicalDevice = static_cast<VkPhysicalDevice>(m_Device->GetPhysicalDevice());
        initInfo.Device = static_cast<VkDevice>(m_Device->GetDevice());
        initInfo.QueueFamily = m_Device->GetGraphicsQueueIndex();
        initInfo.Queue = VulkanHelpers::GetQueueHandle(static_cast<VkDevice>(m_Device->GetDevice()), m_Device->GetGraphicsQueueIndex());
        initInfo.Subpass = 0; // unsure
        initInfo.DescriptorPool = m_ImGuiDescriptorPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
	    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&initInfo);

        //ImGui_ImplVulkan_CreateFontsTexture();

        m_Device->WaitIdle();
    }

    void GUIVulkan::NewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
    }

    void GUIVulkan::Render()
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_ContextHandle->GetSwapchain()->GetCurrentFrame().CommandBuffer);
    }  

    void GUIVulkan::Shutdown()
    {
        m_Device->WaitIdle();
        
        ImGui_ImplVulkan_Shutdown();
        
        vkDestroyDescriptorPool(static_cast<VkDevice>(m_Device->GetDevice()), m_ImGuiDescriptorPool, nullptr);
    }
}