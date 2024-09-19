#include "VulkanPipeline.h"

#include "Renderer/Renderer.h"
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanHelpers.h"

namespace pxl
{
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const std::shared_ptr<VulkanDevice>& device, const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders, const std::shared_ptr<VulkanRenderPass>& renderPass)
        : m_Device(static_cast<VkDevice>(device->GetLogical())), m_Shaders(shaders)
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages; // Store these for graphics pipeline creation

        // Create Shader Stages
        for (auto& shader : m_Shaders)
        {
            VkPipelineShaderStageCreateInfo shaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            shaderStageInfo.stage = ToVkShaderStage(shader.first); // What stage in the graphics pipeline (vertex, geometry, fragment, etc)
            shaderStageInfo.module = static_pointer_cast<VulkanShader>(shader.second)->GetShaderModule();
            shaderStageInfo.pName = "main";                // name of the entrypoint function in the shader
            shaderStageInfo.pSpecializationInfo = nullptr; // this is used to specify values for constants in the shader, so it can perform optimizations such as removing unnecessary if statements

            shaderStages.push_back(shaderStageInfo);
        }

        // Dynamic state
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        auto bindingDescription = VulkanBuffer::GetBindingDescription(specs.VertexLayout);
        auto attributeDescriptions = VulkanBuffer::GetAttributeDescriptions(specs.VertexLayout);

        // Vertex Input
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyInfo.topology = ToVkPrimitiveTopology(specs.PrimitiveType);
        //inputAssemblyInfo.primitiveRestartEnable = VK_FALSE; // useful for strip topology modes

        // Specify viewport state
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1; // } increasing these requires enabling a gpu feature
        viewportState.scissorCount = 1;  // }

        // Rasterization
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationInfo.depthClampEnable = VK_FALSE;                          // requires enabling a gpu feature
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;                   // disables geometry passing this stage, we don't want that
        rasterizationInfo.polygonMode = ToVkPolygonMode(specs.PolygonFillMode); // Can be lines and points, but requires enabling a gpu feature
        rasterizationInfo.lineWidth = 1.0f;                                     // 1.0f is a good default, any higher requires enabling a gpu feature
        rasterizationInfo.cullMode = ToVkCullMode(specs.CullMode);              // specify different types of culling here
        rasterizationInfo.frontFace = ToVkFrontFace(specs.FrontFace);           // This is counter clockwise in OpenGL
        // rasterizationInfo.depthBiasEnable = VK_FALSE;
        // rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        // rasterizationInfo.depthBiasClamp = 0.0f; // Optional
        // rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

        // Multisampling // REQUIRES ENABLING A GPU FEATURE
        VkPipelineMultisampleStateCreateInfo multisamplingInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        // multisamplingInfo.sampleShadingEnable = VK_FALSE; // enable or disable multisampling // NOTE: requires enabling a gpu feature
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        // multisamplingInfo.minSampleShading = 1.0f; // Optional
        // multisamplingInfo.pSampleMask = nullptr; // Optional
        // multisamplingInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        // multisamplingInfo.alphaToOneEnable = VK_FALSE; // Optional

        // Depth/Stencil testing
        // TODO LATER

        // Colour Blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {}; // ColorBlendAttachment is per framebuffer, and ColorBlendState is global // no sType for this struct
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;           // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;                            // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;                 // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;                // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;                            // Optional
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        // Pipeline layout (uniforms, etc)
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 0;            // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        // Create push constant ranges if a pc layout has been provided
        std::vector<VkPushConstantRange> ranges;
        if (specs.PushConstantLayout.has_value())
        {
            for (const auto& element : specs.PushConstantLayout->GetElements())
            {
                VkPushConstantRange range = {};
                range.stageFlags = ToVkShaderStage(element.ShaderStage);
                range.offset = 0; // TODO: implement correct offsets
                range.size = SizeOfUniformDataType(element.Type);

                m_PushConstantRanges[element.Name] = range;
                ranges.push_back(range);
            }

            pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());
            pipelineLayoutInfo.pPushConstantRanges = ranges.data();
        }

        // Create pipeline layout
        VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_Layout));

        // Specify graphics pipeline create info
        VkGraphicsPipelineCreateInfo graphicsPipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
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
        graphicsPipelineInfo.layout = m_Layout;
        graphicsPipelineInfo.renderPass = renderPass->GetVKRenderPass();
        graphicsPipelineInfo.subpass = 0;                         // index of sub pass
        graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // } Used for deriving off previous graphics pipelines, which is less expensive.
        graphicsPipelineInfo.basePipelineIndex = -1;              // } VK_PIPELINE_CREATE_DERIVATIVE_BIT must be defined in the flags for this to work.

        VK_CHECK(vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &m_Pipeline)); // A pipeline cache can be passed to reuse data across multiple calls to vkCreateGraphicsPipelines

        VulkanDeletionQueue::Add([&]()
        {
            Destroy();
        });
    }

    void VulkanGraphicsPipeline::Bind()
    {
        PXL_PROFILE_SCOPE;

        m_CurrentCommandBuffer = static_pointer_cast<VulkanGraphicsContext>(Renderer::GetGraphicsContext())->GetSwapchain()->GetCurrentFrame().CommandBuffer;

        // Bind Pipeline
        vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
    }

    void VulkanGraphicsPipeline::SetUniformData([[maybe_unused]] const std::string& name, [[maybe_unused]] UniformDataType type, [[maybe_unused]] const void* data)
    {
        PXL_PROFILE_SCOPE;
    }

    void VulkanGraphicsPipeline::SetUniformData([[maybe_unused]] const std::string& name, [[maybe_unused]] UniformDataType type, [[maybe_unused]] uint32_t count, [[maybe_unused]] const void* data)
    {
    }

    void VulkanGraphicsPipeline::SetPushConstantData(const std::string& name, const void* data)
    {
        auto range = m_PushConstantRanges.at(name);
        vkCmdPushConstants(m_CurrentCommandBuffer, m_Layout, range.stageFlags, range.offset, range.size, data);
    }

    void VulkanGraphicsPipeline::Destroy()
    {
        if (m_Pipeline)
        {
            vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
            m_Pipeline = VK_NULL_HANDLE;
        }

        if (m_Layout)
        {
            vkDestroyPipelineLayout(m_Device, m_Layout, nullptr);
            m_Layout = VK_NULL_HANDLE;
        }
    }

    VkShaderStageFlagBits VulkanGraphicsPipeline::ToVkShaderStage(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:       return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderStage::Fragment:     return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderStage::Geometry:     return VK_SHADER_STAGE_GEOMETRY_BIT;
            case ShaderStage::Tessellation: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; // VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
        }

        PXL_LOG_WARN(LogArea::Vulkan, "Invalid VkShaderStage");

        return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }

    VkPrimitiveTopology VulkanGraphicsPipeline::ToVkPrimitiveTopology(PrimitiveTopology topology)
    {
        switch (topology)
        {
            case PrimitiveTopology::Triangle:      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case PrimitiveTopology::TriangleFan:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case PrimitiveTopology::Line:          return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case PrimitiveTopology::LineStrip:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case PrimitiveTopology::Point:         return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }

        PXL_LOG_WARN(LogArea::Vulkan, "Invalid PrimitiveTopology");

        return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    }

    VkPolygonMode VulkanGraphicsPipeline::ToVkPolygonMode(PolygonFillMode mode)
    {
        switch (mode)
        {
            case PolygonFillMode::Fill: return VK_POLYGON_MODE_FILL;
        }

        PXL_LOG_WARN(LogArea::Vulkan, "Invalid PolygonFillMode");

        return VK_POLYGON_MODE_MAX_ENUM;
    }

    VkCullModeFlagBits VulkanGraphicsPipeline::ToVkCullMode(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None:  return VK_CULL_MODE_NONE;
            case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back:  return VK_CULL_MODE_BACK_BIT;
        }

        PXL_LOG_WARN(LogArea::Vulkan, "Invalid CullMode");

        return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
    }

    VkFrontFace VulkanGraphicsPipeline::ToVkFrontFace(FrontFace face)
    {
        switch (face)
        {
            case FrontFace::CW:  return VK_FRONT_FACE_CLOCKWISE;
            case FrontFace::CCW: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }

        PXL_LOG_WARN(LogArea::Vulkan, "Invalid FrontFace");

        return VK_FRONT_FACE_MAX_ENUM;
    }
}