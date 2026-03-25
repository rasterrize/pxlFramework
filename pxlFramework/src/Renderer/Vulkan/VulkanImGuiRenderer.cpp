#include "VulkanImGuiRenderer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "VulkanGraphicsDevice.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanImGuiRenderer::VulkanImGuiRenderer(
        const ImGuiSpecs& specs,
        VkInstance instance,
        VkPhysicalDevice gpu,
        VkDevice device,
        VkQueue queue,
        VkFormat format,
        uint32_t imageCount)
        : ImGuiRenderer(specs), m_Device(device)
    {
        // Create descriptor pool
        // NOTE: The sizes are purposefully oversize
        VkDescriptorPoolSize poolSizes[] = {
            {                VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            {          VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            {          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            {   VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            {   VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            {         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            {         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            {       VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = std::size(poolSizes);
        poolInfo.pPoolSizes = poolSizes;

        VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_Pool));

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        // Set custom font
        const char* fontFilename = "resources/fonts/Roboto-Medium.ttf";
        std::filesystem::exists(fontFilename) ? io.Fonts->AddFontFromFileTTF(fontFilename, 16) : io.Fonts->AddFontDefault();

        ImGui_ImplGlfw_InitForVulkan(m_Specs.Window->GetNativeWindow(), true);

        // Pipeline rendering info (for dynamic rendering)
        VkPipelineRenderingCreateInfo pipelineRenderingInfo = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
        pipelineRenderingInfo.colorAttachmentCount = 1;
        pipelineRenderingInfo.pColorAttachmentFormats = &format;

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.ApiVersion = VK_API_VERSION_1_3;
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = pipelineRenderingInfo;
        initInfo.Instance = instance;
        initInfo.PhysicalDevice = gpu;
        initInfo.Device = device;
        initInfo.Queue = queue;
        initInfo.DescriptorPool = m_Pool;
        initInfo.MinImageCount = imageCount;
        initInfo.ImageCount = imageCount;

        ImGui_ImplVulkan_Init(&initInfo);
    }

    void VulkanImGuiRenderer::Free()
    {
        ImGui_ImplVulkan_Shutdown();

        if (m_Pool)
        {
            vkDestroyDescriptorPool(m_Device, m_Pool, nullptr);
            m_Pool = VK_NULL_HANDLE;
        }

        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    void VulkanImGuiRenderer::NewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void VulkanImGuiRenderer::Render(const std::unique_ptr<GraphicsDevice>& device)
    {
        ImGui::Render();

        VulkanGraphicsDevice* vulkanDevice = dynamic_cast<VulkanGraphicsDevice*>(device.get());
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vulkanDevice->GetCurrentFrame().CommandBuffer);
    }
}