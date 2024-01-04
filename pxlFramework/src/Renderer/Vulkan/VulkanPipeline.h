#pragma once

#include "../Pipeline.h"

#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "../BufferLayout.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(VkDevice device, const std::shared_ptr<VulkanShader>& shader, const std::shared_ptr<VulkanRenderPass> renderPass, const BufferLayout& bufferLayout);
        ~VulkanGraphicsPipeline();

        void Bind(VkCommandBuffer commandBuffer);

        virtual void* GetPipelineLayout() override { return m_Layout; }

        virtual void Destroy() override;

        VkPipeline GetVKPipeline() const { return m_Pipeline; }

        VkViewport GetViewport() const { return m_Viewport; }
        VkRect2D GetScissor() const { return m_Scissor; }

        void SetViewport(const VkViewport& viewport) { m_Viewport = viewport; }
        void SetScissor(VkRect2D scissor) { m_Scissor = scissor; }

        void ResizeViewport(VkExtent2D extent) { m_Viewport.width = extent.width; m_Viewport.height = extent.height; } // probably shouldnt be taking Extent since thats not what the viewport has
        void ResizeScissor(VkExtent2D extent) { m_Scissor.extent = extent; }
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;

        // Dynamic state // TODO: This probably shouldn't be fixed and be a dynamic dynamic state
        VkViewport m_Viewport = {};
        VkRect2D m_Scissor = {};
    };
}