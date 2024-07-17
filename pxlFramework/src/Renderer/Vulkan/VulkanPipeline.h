#pragma once

#include "../Pipeline.h"

#include <vulkan/vulkan.h>

#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "../BufferLayout.h"

namespace pxl
{
    struct PushConstantData
    {
        std::string Name;
        const void* Data;
    };

    struct PushConstantRange
    {
        std::string Name;
        VkPushConstantRange Range;
    };

    struct VulkanGraphicsPipelineSettings
    {
        VkPolygonMode PolygonMode = VK_POLYGON_MODE_FILL;
        VkFrontFace FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // like OpenGL
        VkCullModeFlags CullMode = VK_CULL_MODE_BACK_BIT;
    };
    
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders, const std::shared_ptr<VulkanRenderPass>& renderPass);
        virtual ~VulkanGraphicsPipeline() override;

        virtual void Bind() override;
        virtual void Unbind() override {};
        
        virtual void SetUniformData(const std::string& name, BufferDataType type, const void* data) override;
        virtual void SetPushConstantData(const std::string& name, const void* data) override;

        virtual void* GetPipelineLayout() override { return m_Layout; }

        void Destroy();

        VkPipeline GetVKPipeline() const { return m_Pipeline; }

    private:
        static VkShaderStageFlagBits ToVkShaderStage(ShaderStage stage);
        static VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveTopology topology);
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;
        VkCommandBuffer m_CurrentCommandBuffer = VK_NULL_HANDLE;

        std::unordered_map<ShaderStage, std::shared_ptr<Shader>> m_Shaders; // NOTE: currently holds on to them so they don't immediately get destroyed
        std::unordered_map<std::string, VkPushConstantRange> m_PushConstantRanges;

        VulkanGraphicsPipelineSettings m_Settings;
    };
}