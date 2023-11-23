#pragma once

#include "../Pipeline.h"

#include "../Shader.h"

#include "VulkanRenderPass.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(VkDevice device, const std::shared_ptr<VulkanShader>& shader, const std::shared_ptr<VulkanRenderPass> renderPass);
        ~VulkanGraphicsPipeline();

        virtual void* GetPipelineLayout() override { return m_Layout; }

        virtual void Destroy() override;

        VkPipeline GetVKPipeline() const { return m_Pipeline; }

        VkViewport GetViewport() const { return m_Viewport; }  /*RecreateGraphicsPipeline() idk*/
        VkRect2D GetScissor() const { return m_Scissor; }

        void SetViewport(VkViewport viewport) { m_Viewport = viewport;  /*RecreateGraphicsPipeline() idk*/}
        void SetScissor(VkRect2D scissor) { m_Scissor = scissor; }
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;

        // Dynamic state // TODO: This probably shouldn't be fixed and be a dynamic dynamic state
        VkViewport m_Viewport = {};
        VkRect2D m_Scissor = {};
    };
}