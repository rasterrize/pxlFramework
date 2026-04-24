#include "VulkanBindlessTextureHandler.h"

#include "VulkanTexture.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanBindlessTextureHandler::VulkanBindlessTextureHandler(VkDevice device)
        : m_Device(device)
    {
        // Create descriptor set layout
        // Descriptor indexing uses variable descriptor counts
        VkDescriptorBindingFlags bindingFlags = { VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT };
        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
        bindingFlagsInfo.bindingCount = 1;
        bindingFlagsInfo.pBindingFlags = &bindingFlags;

        VkDescriptorSetLayoutBinding descLayoutBindingTex = {};
        descLayoutBindingTex.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descLayoutBindingTex.descriptorCount = m_MaxTextureCount;
        descLayoutBindingTex.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descLayoutBindingTex.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo setLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        setLayoutInfo.flags = 0;
        setLayoutInfo.pNext = &bindingFlagsInfo;
        setLayoutInfo.bindingCount = 1;
        setLayoutInfo.pBindings = &descLayoutBindingTex;

        VK_CHECK(vkCreateDescriptorSetLayout(m_Device, &setLayoutInfo, nullptr, &m_SetLayout));

        // Create descriptor pool
        VkDescriptorPoolSize poolSize;
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = m_MaxTextureCount;

        VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.maxSets = 1;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;

        VK_CHECK(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool));

        // Allocate descriptor sets from descriptor pool
        VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO };
        variableDescriptorAllocInfo.descriptorSetCount = 1;
        variableDescriptorAllocInfo.pDescriptorCounts = &m_MaxTextureCount;

        VkDescriptorSetAllocateInfo texDescSetAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        texDescSetAllocInfo.pNext = &variableDescriptorAllocInfo;
        texDescSetAllocInfo.descriptorPool = m_DescriptorPool;
        texDescSetAllocInfo.descriptorSetCount = 1;
        texDescSetAllocInfo.pSetLayouts = &m_SetLayout;

        VK_CHECK(vkAllocateDescriptorSets(m_Device, &texDescSetAllocInfo, &m_DescriptorSet));
    }

    void VulkanBindlessTextureHandler::Free()
    {
        if (m_DescriptorPool)
        {
            vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
            m_DescriptorPool = VK_NULL_HANDLE;
            m_DescriptorSet = VK_NULL_HANDLE;
        }

        if (m_SetLayout)
        {
            vkDestroyDescriptorSetLayout(m_Device, m_SetLayout, nullptr);
            m_SetLayout = VK_NULL_HANDLE;
        }
    }
    void VulkanBindlessTextureHandler::Add(std::shared_ptr<Texture> texture)
    {
        PXL_ASSERT(texture);

        bool found = false;
        for (size_t i = 0; i < m_Textures.size(); i++)
        {
            if (m_Textures.at(i) == texture)
            {
                found = true;
                return;
            }
        }

        if (!found)
        {
            m_Textures.push_back(texture);
            m_NeedsUpload = true;
        }
    }

    uint32_t VulkanBindlessTextureHandler::GetIndex(const std::shared_ptr<Texture>& texture)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(m_Textures.size()); i++)
        {
            if (m_Textures.at(i) == texture)
                return i;
        }

        return UINT32_MAX;
    }

    void VulkanBindlessTextureHandler::Upload()
    {
        if (m_Textures.empty())
            return;

        std::vector<VkDescriptorImageInfo> imageData;
        for (const auto& texture : m_Textures)
        {
            auto vulkanTexture = static_pointer_cast<VulkanTexture>(texture);

            VkDescriptorImageInfo imageInfo;
            imageInfo.sampler = vulkanTexture->GetSampler();
            imageInfo.imageView = vulkanTexture->GetImageView();
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageData.push_back(imageInfo);
        }

        VkWriteDescriptorSet writeSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        writeSet.dstSet = m_DescriptorSet;
        writeSet.dstBinding = 0;
        writeSet.descriptorCount = static_cast<uint32_t>(imageData.size());
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeSet.pImageInfo = imageData.data();

        vkUpdateDescriptorSets(m_Device, 1, &writeSet, 0, nullptr);

        m_NeedsUpload = false;
    }
}