#include "ImGuiVulkan.h"

#include <backends/imgui_impl_vulkan.h>

#include "../../Renderer/Vulkan/VulkanHelpers.h"

namespace pxl
{
    ImGuiVulkan::ImGuiVulkan(const std::shared_ptr<GraphicsContext>& context)
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
	    pool_info.poolSizeCount = std::size(pool_sizes);
	    pool_info.pPoolSizes = pool_sizes;

	    VK_CHECK(vkCreateDescriptorPool(m_Device->GetVkDevice(), &pool_info, nullptr, &m_ImGuiDescriptorPool));

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = m_ContextHandle->GetInstance();
        initInfo.PhysicalDevice = m_Device->GetVkPhysicalDevice();
        initInfo.Device = m_Device->GetVkDevice();
        initInfo.QueueFamily = m_Device->GetGraphicsQueueIndex();
        initInfo.Queue = VulkanHelpers::GetQueueHandle(m_Device->GetVkDevice(), m_Device->GetGraphicsQueueIndex());
        initInfo.Subpass = 0; // unsure
        initInfo.DescriptorPool = m_ImGuiDescriptorPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
	    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&initInfo);

        //ImGui_ImplVulkan_CreateFontsTexture();

        m_Device->WaitIdle();
    }

    void ImGuiVulkan::NewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
    }

    void ImGuiVulkan::Render()
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_ContextHandle->GetSwapchain()->GetCurrentFrame().CommandBuffer);
    }  

    void ImGuiVulkan::Shutdown()
    {
        m_Device->WaitIdle();
        
        ImGui_ImplVulkan_Shutdown();
        
        vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_ImGuiDescriptorPool, nullptr);
    }
}