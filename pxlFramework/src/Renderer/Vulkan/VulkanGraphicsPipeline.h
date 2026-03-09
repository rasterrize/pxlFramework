#pragma once

#include "Renderer/GraphicsPipeline.h"

#include <volk/volk.h>

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const GraphicsPipelineSpecs& specs, VkDevice device, VkFormat format);

        virtual void Recreate() override;

        virtual void Free() override;

        VkPipeline GetVkPipeline() const { return m_Pipeline; }

        static VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout);
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(const BufferLayout& layout);

    private:
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkFormat m_RenderingFormat = VK_FORMAT_UNDEFINED;
    };
}