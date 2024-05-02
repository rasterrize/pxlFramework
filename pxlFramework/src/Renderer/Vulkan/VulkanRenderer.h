#pragma once

#include "../RendererAPI.h"

#include <vulkan/vulkan.h>

#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"

namespace pxl
{
    class VulkanRenderer : public RendererAPI
    {
    public:
        VulkanRenderer(const std::shared_ptr<VulkanGraphicsContext>& context);
        virtual ~VulkanRenderer() override;

        virtual void Begin() override;
        virtual void End() override;
        
        virtual void Clear() override {};
        virtual void SetClearColour(const glm::vec4& colour) override { m_ClearValue.color = { colour.r, colour.g, colour.b, colour.a }; }

        virtual void DrawArrays(uint32_t vertexCount) override;
        virtual void DrawLines(uint32_t vertexCount) override;
        virtual void DrawIndexed(uint32_t indexCount) override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        VkViewport GetViewport() const { return m_Viewport; }
        VkRect2D GetScissor() const { return m_Scissor; }

        void Destroy();
    private:
        std::shared_ptr<VulkanDevice> m_Device = nullptr;
        std::shared_ptr<VulkanGraphicsContext> m_ContextHandle = nullptr;

        VkClearValue m_ClearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
        VkViewport m_Viewport = {};
        VkRect2D m_Scissor = {};

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        VulkanFrame m_CurrentFrame;

        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;

        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass;
    };
}