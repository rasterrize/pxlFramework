#include "VulkanGraphicsPipeline.h"

#include "VulkanBindlessTextureHandler.h"
#include "VulkanShader.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const GraphicsPipelineSpecs& specs, VkDevice device, VkFormat renderingFormat)
        : m_Specs(specs), m_Device(device), m_RenderingFormat(renderingFormat)
    {
        Recreate();
    }

    void VulkanGraphicsPipeline::Recreate()
    {
        // Wait for any GPU work to finish
        VK_CHECK(vkDeviceWaitIdle(m_Device));

        // Free any existing resources
        Free();

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages; // Store these for graphics pipeline creation

        // Create Shader Stages
        for (auto& [stage, shader] : m_Specs.Shaders)
        {
            VkPipelineShaderStageCreateInfo shaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            shaderStageInfo.stage = VulkanUtils::ToVkShaderStage(stage);
            shaderStageInfo.module = static_pointer_cast<VulkanShader>(shader)->GetModule();
            shaderStageInfo.pName = "main";
            shaderStageInfo.pSpecializationInfo = nullptr;

            shaderStages.push_back(shaderStageInfo);
        }

        // Dynamic state
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_CULL_MODE,
            VK_DYNAMIC_STATE_FRONT_FACE,
            VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        // Vertex Input
        auto bindingDescription = GetBindingDescription(m_Specs.BufferLayout);
        auto attributeDescriptions = GetAttributeDescriptions(m_Specs.BufferLayout);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyInfo.topology = VulkanUtils::ToVkPrimitiveTopology(m_Specs.PrimitiveTopology);
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // Specify viewport state
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterization
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationInfo.depthClampEnable = VK_FALSE;                                     // requires enabling a gpu feature
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;                              // disables geometry passing this stage, we don't want that
        rasterizationInfo.polygonMode = VulkanUtils::ToVkPolygonMode(m_Specs.PolygonMode); // Can be lines and points, but requires enabling a gpu feature
        rasterizationInfo.cullMode = VulkanUtils::ToVkCullMode(m_Specs.CullMode);
        rasterizationInfo.frontFace = VulkanUtils::ToVkFrontFace(m_Specs.FrontFace);
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.lineWidth = 1.0f;

        // Multisampling // REQUIRES ENABLING A GPU FEATURE
        VkPipelineMultisampleStateCreateInfo multisamplingInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisamplingInfo.sampleShadingEnable = VK_FALSE;
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Depth/Stencil testing
        // TODO LATER

        // Colour Blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {}; // ColorBlendAttachment is per framebuffer, and ColorBlendState is global
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        // Pipeline rendering info (for dynamic rendering)
        VkPipelineRenderingCreateInfo pipelineRenderingInfo = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
        pipelineRenderingInfo.colorAttachmentCount = 1;
        pipelineRenderingInfo.pColorAttachmentFormats = &m_RenderingFormat;

        std::vector<VkPushConstantRange> pushConstants;
        if (m_Specs.UniformLayout.has_value())
        {
            // TODO: Layout details aren't used in here, should specs be reevaluated?
            VkPushConstantRange range = {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .size = sizeof(VkDeviceAddress),
            };

            pushConstants.push_back(range);
        }

        std::vector<VkDescriptorSetLayout> descriptorLayouts;
        if (m_Specs.TextureHandler)
        {
            auto bindlessHandler = static_pointer_cast<VulkanBindlessTextureHandler>(m_Specs.TextureHandler);
            descriptorLayouts.push_back(bindlessHandler->GetDescriptorLayout());
        }

        // Pipeline layout (uniforms, etc)
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout));

        VkGraphicsPipelineCreateInfo graphicsPipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        graphicsPipelineInfo.pNext = &pipelineRenderingInfo;
        graphicsPipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        graphicsPipelineInfo.pStages = shaderStages.data();
        graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
        graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        graphicsPipelineInfo.pViewportState = &viewportState;
        graphicsPipelineInfo.pRasterizationState = &rasterizationInfo;
        graphicsPipelineInfo.pMultisampleState = &multisamplingInfo;
        graphicsPipelineInfo.pDepthStencilState = nullptr;
        graphicsPipelineInfo.pColorBlendState = &colorBlending;
        graphicsPipelineInfo.pDynamicState = &dynamicStateInfo;
        graphicsPipelineInfo.layout = m_PipelineLayout;
        graphicsPipelineInfo.renderPass = VK_NULL_HANDLE; // We are using dynamic rendering so this can just be null
        graphicsPipelineInfo.subpass = 0;
        graphicsPipelineInfo.basePipelineHandle = m_Pipeline; // Used for deriving off previous graphics pipelines, which is less expensive.
        graphicsPipelineInfo.basePipelineIndex = -1;          // VK_PIPELINE_CREATE_DERIVATIVE_BIT must be defined in the flags for this to work.

        // A pipeline cache can be passed to reuse data across multiple calls to vkCreateGraphicsPipelines
        VK_CHECK(vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &m_Pipeline));
    }

    void VulkanGraphicsPipeline::Free()
    {
        if (m_PipelineLayout)
        {
            vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
            m_PipelineLayout = VK_NULL_HANDLE;
        }

        if (m_Pipeline)
        {
            vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
            m_Pipeline = VK_NULL_HANDLE;
        }
    }

    VkVertexInputBindingDescription VulkanGraphicsPipeline::GetBindingDescription(const BufferLayout& layout)
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = layout.GetStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanGraphicsPipeline::GetAttributeDescriptions(const BufferLayout& layout)
    {
        auto& elements = layout.Elements;
        std::vector<VkVertexInputAttributeDescription> vertexAttributes(elements.size());

        uint32_t offset = 0;
        for (size_t i = 0; i < elements.size(); i++)
        {
            vertexAttributes.at(i).binding = 0;
            vertexAttributes.at(i).format = VulkanUtils::ToVkFormat(elements.at(i));
            vertexAttributes.at(i).location = static_cast<uint32_t>(i);
            vertexAttributes.at(i).offset = offset;
            offset += Utils::SizeOfBufferDataType(elements.at(i));
        }

        return vertexAttributes;
    }
}