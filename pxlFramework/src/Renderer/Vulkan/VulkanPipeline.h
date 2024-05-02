#pragma once

#include "../Pipeline.h"

#include <vulkan/vulkan.h>

#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "../BufferLayout.h"

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const std::shared_ptr<VulkanShader>& shader, const std::shared_ptr<VulkanRenderPass> renderPass, const BufferLayout& bufferLayout);
        virtual ~VulkanGraphicsPipeline() override;

        virtual void Bind() override;

        virtual void* GetPipelineLayout() override { return m_Layout; }

        virtual void Destroy() override;

        VkPipeline GetVKPipeline() const { return m_Pipeline; }
    private:
        std::shared_ptr<VulkanGraphicsContext> m_ContextHandle;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    };
}