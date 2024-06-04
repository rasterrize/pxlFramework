#pragma once

#include "../Pipeline.h"

#include <vulkan/vulkan.h>

#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
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
        VulkanGraphicsPipeline(std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders, const std::shared_ptr<VulkanRenderPass>& renderPass, const BufferLayout& bufferLayout, const UniformLayout& uniformLayout);
        virtual ~VulkanGraphicsPipeline() override;

        virtual void Bind() override;
        virtual void SetPushConstantData(std::unordered_map<std::string, const void*>& pcData) override;

        virtual void* GetPipelineLayout() override { return m_Layout; }

        virtual void Destroy() override;

        VkPipeline GetVKPipeline() const { return m_Pipeline; }

    private:
        static VkShaderStageFlagBits GetVkShaderStage(ShaderStage stage);
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;

        std::unordered_map<ShaderStage, std::shared_ptr<Shader>> m_Shaders; // NOTE: currently holds on to them so they don't immediately get destroyed
        std::vector<PushConstantRange> m_PushConstants;

        VulkanGraphicsPipelineSettings m_Settings;
    };
}