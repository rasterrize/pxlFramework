#pragma once

#include "../Pipeline.h"

#include "../GraphicsContext.h"
#include "../Shader.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const std::shared_ptr<GraphicsContext>& context, std::shared_ptr<Shader>& shader);
        ~VulkanGraphicsPipeline();

        virtual void* GetPipelineLayout() override { return m_Layout; }
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;

        // Dynamic state // TODO: This probably shouldn't be fixed and be a dynamic dynamic state
        VkViewport m_Viewport = {};
        VkRect2D m_Scissor = {};
    };
}