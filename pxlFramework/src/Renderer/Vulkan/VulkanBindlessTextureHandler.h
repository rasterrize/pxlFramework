#pragma once

#include <volk/volk.h>

#include "Renderer/TextureHandler.h"

namespace pxl
{
    class VulkanBindlessTextureHandler : public TextureHandler
    {
    public:
        VulkanBindlessTextureHandler(VkDevice device);

        virtual void Free() override;

        virtual void Add(std::shared_ptr<Texture> texture) override;

        virtual uint32_t GetIndex(const std::shared_ptr<Texture>& texture) override;

        virtual void Upload() override;

        virtual bool NeedsUpload() const override { return m_NeedsUpload; }

        virtual bool IsFull() const override { return false; }

        virtual void Reset() override { m_Textures.clear(); }

        VkDescriptorSetLayout GetDescriptorLayout() const { return m_SetLayout; }
        VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

    private:
        std::vector<std::shared_ptr<Texture>> m_Textures;
        const uint32_t m_MaxTextureCount = 32;

        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_SetLayout = VK_NULL_HANDLE;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;

        bool m_NeedsUpload = false;
    };
}