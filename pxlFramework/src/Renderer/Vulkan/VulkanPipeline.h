#pragma once

#include "../Pipeline.h"

#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "../BufferLayout.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const std::shared_ptr<VulkanContext>& context, const std::shared_ptr<VulkanDevice>& device, const std::shared_ptr<VulkanShader>& shader, const std::shared_ptr<VulkanRenderPass> renderPass, const BufferLayout& bufferLayout);
        ~VulkanGraphicsPipeline();

        virtual void Bind() override;

        virtual void* GetPipelineLayout() override { return m_Layout; }

        virtual void Destroy() override;

        VkPipeline GetVKPipeline() const { return m_Pipeline; }
    private:
        std::shared_ptr<VulkanContext> m_ContextHandle;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    };
}