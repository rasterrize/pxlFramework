#pragma once

#include "../RendererAPI.h"

#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanRenderer : public RendererAPI
    {
    public:
        VulkanRenderer(const std::shared_ptr<VulkanContext>& context);
        ~VulkanRenderer();

        virtual void Begin() override;
        virtual void End() override;
        
        virtual void Clear() override {};
        virtual void SetClearColour(const glm::vec4& colour) override { m_ClearValue.color = { colour.r, colour.b, colour.g, colour.a }; }

        virtual void DrawArrays(uint32_t vertexCount) override;
        virtual void DrawLines(uint32_t vertexCount) override;
        virtual void DrawIndexed(uint32_t indexCount) override;

        void Destroy();
    private:
        std::shared_ptr<VulkanContext> m_ContextHandle = nullptr;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
        VkClearValue m_ClearValue = { { { 20.0f / 255.0f, 24.0f / 255.0f, 28.0f / 255.0f, 1.0f } } };

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence m_InFlightFence = VK_NULL_HANDLE; // We are using 1 command buffer currently, so we must wait until its operations are done before we can use the command buffers and semaphores again.

        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;

        // TODO: these should be moved outside of this class and passed in through function parameters, especially pipelines, since thats how switching of shaders works
        std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
        std::shared_ptr<VulkanRenderPass> m_RenderPass;
        std::shared_ptr<VulkanShader> m_Shader;

        std::shared_ptr<VulkanBuffer> m_TestVertexBuffer;
        std::shared_ptr<VulkanBuffer> m_TestIndexBuffer;
    };
}