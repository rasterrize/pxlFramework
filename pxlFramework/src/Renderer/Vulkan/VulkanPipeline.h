#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/BufferLayout.h"
#include "Renderer/Pipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"

namespace pxl
{
    struct PushConstantData
    {
        std::string Name;
        const void* Data = nullptr;
    };

    struct PushConstantRange
    {
        std::string Name;
        VkPushConstantRange Range = {};
    };

    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const std::shared_ptr<VulkanDevice>& device, const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders, const std::shared_ptr<VulkanRenderPass>& renderPass);

        virtual void Bind() override;
        virtual void Unbind() override {};

        virtual void SetUniformData(const std::string& name, UniformDataType type, const void* data) override;
        virtual void SetUniformData(const std::string& name, UniformDataType type, uint32_t count, const void* data) override;
        virtual void SetPushConstantData(const std::string& name, const void* data) override;

        virtual void* GetPipelineLayout() override { return m_Layout; }

        void Destroy();

        VkPipeline GetVKPipeline() const { return m_Pipeline; }
    private:
        static VkShaderStageFlagBits ToVkShaderStage(ShaderStage stage);
        static VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveTopology topology);
        static VkPolygonMode ToVkPolygonMode(PolygonMode mode);
        static VkCullModeFlagBits ToVkCullMode(CullMode mode);
        static VkFrontFace ToVkFrontFace(FrontFace face);
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_Layout = VK_NULL_HANDLE;
        VkCommandBuffer m_CurrentCommandBuffer = VK_NULL_HANDLE;

        std::unordered_map<ShaderStage, std::shared_ptr<Shader>> m_Shaders; // NOTE: currently holds on to them so they don't immediately get destroyed
        std::unordered_map<std::string, VkPushConstantRange> m_PushConstantRanges;
    };
}