#pragma once

#include <volk/volk.h>

#include "Renderer/GraphicsPipeline.h"

namespace pxl
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const GraphicsPipelineSpecs& specs, VkDevice device, VkFormat format);

        virtual void Recreate() override;

        virtual void Free() override;

        virtual const GraphicsPipelineSpecs& GetSpecs() const { return m_Specs; }

        VkPipeline GetVkPipeline() const { return m_Pipeline; }
        VkPipelineLayout GetVkPipelineLayout() const { return m_PipelineLayout; }

        static VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout);
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(const BufferLayout& layout);

    private:
        GraphicsPipelineSpecs m_Specs;

        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkFormat m_RenderingFormat = VK_FORMAT_UNDEFINED;

        VkDeviceAddress m_UniformBufferAddress = UINT64_MAX;
    };
}