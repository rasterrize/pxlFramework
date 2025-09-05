#pragma once

#include <Volk/volk.h>

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"
#include "VulkanDevice.h"
#include "VulkanHelpers.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"

namespace pxl
{
    class VulkanGraphicsContext : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(const std::shared_ptr<Window>& window);

        virtual void Present() override;

        virtual bool GetVSync() const override { return m_Swapchain->GetVSync(); }
        virtual void SetVSync(bool value) override
        {
            m_Swapchain->SetVSync(value);
            m_Swapchain->Recreate();
        }
        virtual void ToggleVSync() { SetVSync(!m_Swapchain->GetVSync()); }

        virtual void SetAsCurrent() override {};

        virtual std::shared_ptr<GraphicsDevice> GetDevice() const override { return m_Device; }

        virtual RendererLimits GetLimits() override { return RendererLimits(); }

        VkSurfaceKHR GetSurface() const { return m_Surface; }
        VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }

        std::shared_ptr<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }

        // TODO: move
        std::shared_ptr<VulkanRenderPass> GetDefaultRenderPass() const { return m_DefaultRenderPass; } // Geometry Render Pass?

    private:
        std::shared_ptr<VulkanDevice> m_Device = nullptr;
        std::shared_ptr<VulkanSwapchain> m_Swapchain = nullptr;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkSurfaceFormatKHR m_SurfaceFormat = {};

        // IDK
        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass = nullptr; // TODO: move this to VulkanRendererAPI
    };
}