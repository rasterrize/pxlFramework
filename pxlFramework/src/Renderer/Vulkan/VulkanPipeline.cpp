#include "VulkanPipeline.h"
#include "VulkanErrorCheck.h"
#include "VulkanShader.h"
#include "VulkanContext.h"

#include "../Shader.h"

namespace pxl
{
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const std::shared_ptr<GraphicsContext>& context, std::shared_ptr<Shader>& shader)
    {
        m_Device = dynamic_pointer_cast<VulkanContext>(context)->GetDevice();
        if (!m_Device)
        {
            Logger::LogError("Failed to retrieve VkDevice from graphics context");
        }

        auto vulkanShader = static_pointer_cast<VulkanShader>(shader);

        VkResult result;

        // Create Shader Stages
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // What stage in the graphics pipeline (vertex, geometry, fragment, etc)
        vertShaderStageInfo.module = vulkanShader->GetShaderModule(VK_SHADER_STAGE_VERTEX_BIT);
        vertShaderStageInfo.pName = "main"; // name of the entrypoint function in the shader
        vertShaderStageInfo.pSpecializationInfo = nullptr; // this is used to specify values for constants in the shader, so it can perform optimizations such as removing unnecessary if statements

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = vulkanShader->GetShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT);
        fragShaderStageInfo.pName = "main";
        fragShaderStageInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo}; // Store these for graphics pipeline creation

        // Dynamic state
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        // Vertex Input // NOTE: this currently doesn't take any vertex input because its specified in the vertex shader, will be changed once using vertex buffers
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        // vertexInputInfo.vertexBindingDescriptionCount = 0;
        // vertexInputInfo.pVertexBindingDescriptions = nullptr;
        // vertexInputInfo.vertexAttributeDescriptionCount = 0;
        // vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        //inputAssemblyInfo.primitiveRestartEnable = VK_FALSE; // useful for strip topology modes

        // Setup Viewport
        m_Viewport.x = 0.0f;
        m_Viewport.y = 0.0f;
        m_Viewport.width = 640; // TODO: width and height should be size of the window
        m_Viewport.height = 480;
        m_Viewport.minDepth = 0.0f;
        m_Viewport.maxDepth = 1.0f;

        // Specify viewport state
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Setup Scissor
        m_Scissor.offset = { 0, 0 };
        m_Scissor.extent = { 640, 480 }; // TODO: Should be window size

        // Rasterization
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        //rasterizationInfo.depthClampEnable = VK_FALSE; // requires enabling a gpu feature
        //rasterizationInfo.rasterizerDiscardEnable = VK_FALSE; // disables geometry passing this stage, we don't want that
        //rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL; // Can be lines and points, but requires enabling a gpu feature
        rasterizationInfo.lineWidth = 1.0f; // 1.0f is good default, any higher requires enabling a gpu feature
        rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT; // specify different types of culling here
        rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        //rasterizationInfo.depthBiasEnable = VK_FALSE;
        // rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        // rasterizationInfo.depthBiasClamp = 0.0f; // Optional
        // rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

        // Multisampling // REQUIRES ENABLING A GPU FEATURE
        VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
        multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        //multisamplingInfo.sampleShadingEnable = VK_FALSE; // enable or disable multisampling // NOTE: requires enabling a gpu feature
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        // multisamplingInfo.minSampleShading = 1.0f; // Optional
        // multisamplingInfo.pSampleMask = nullptr; // Optional
        // multisamplingInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        // multisamplingInfo.alphaToOneEnable = VK_FALSE; // Optional

        // Depth/Stencil testing
        // TODO LATER

        // Colour Blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {}; // ColorBlendAttachment is per framebuffer, and ColorBlendState is global
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        // Pipeline layout (uniforms, etc)
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        // Create pipeline layout
        result = vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_Layout);
        CheckVkResult(result);

        // Specify graphics pipeline create info
        VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {};
        graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineInfo.stageCount = 0;
        graphicsPipelineInfo.pStages = nullptr;
        graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
        graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        graphicsPipelineInfo.pViewportState = &viewportState;
        graphicsPipelineInfo.pRasterizationState = &rasterizationInfo;
        graphicsPipelineInfo.pMultisampleState = &multisamplingInfo;
        graphicsPipelineInfo.pDepthStencilState = nullptr;
        graphicsPipelineInfo.pColorBlendState = &colorBlending;
        graphicsPipelineInfo.pDynamicState = &dynamicStateInfo;
        graphicsPipelineInfo.layout = m_Layout;
        graphicsPipelineInfo.renderPass = nullptr;
        graphicsPipelineInfo.subpass = 0; // index of sub pass
        graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // } Used for deriving off previous graphics pipelines, which is less expensive.
        graphicsPipelineInfo.basePipelineIndex = -1;              // } VK_PIPELINE_CREATE_DERIVATIVE_BIT must be defined in the flags for this to work.

        result = vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &m_Pipeline); // A pipeline cache can be passed to reuse data across multiple calls to vkCreateGraphicsPipelines
        CheckVkResult(result);
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        if (m_Pipeline != VK_NULL_HANDLE)
            vkDestroyPipeline(m_Device, m_Pipeline, nullptr);

        if (m_Layout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(m_Device, m_Layout, nullptr);
    }
}