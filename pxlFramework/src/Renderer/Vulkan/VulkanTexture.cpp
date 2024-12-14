#include "VulkanTexture.h"

#include "Renderer/Renderer.h"

namespace pxl
{
    VulkanTexture::VulkanTexture(const Image& image, const TextureSpecs& specs)
        : m_Device(Renderer::GetGraphicsContext()->GetDevice())
    {
        m_Image = std::make_unique<VulkanImage>(image.Metadata.Size, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT); // TODO: unhardcode format

        m_Image->SetData(image);

        // Create sampler
        VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerInfo.magFilter = ToVkFilter(specs.Filter);
        samplerInfo.minFilter = ToVkFilter(specs.Filter);
        samplerInfo.addressModeU = ToVkAddressMode(specs.WrapMode);
        samplerInfo.addressModeV = ToVkAddressMode(specs.WrapMode);
        samplerInfo.addressModeW = ToVkAddressMode(specs.WrapMode);
        samplerInfo.anisotropyEnable = VK_FALSE; // TODO
        samplerInfo.maxAnisotropy = 0.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // TRUE = 0 to textureWidth/Height range. You need a very specific reason to use this.
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // These 4 are for mipmapping
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VK_CHECK(vkCreateSampler(static_cast<VkDevice>(m_Device->GetLogical()), &samplerInfo, nullptr, &m_Sampler));

        VulkanDeletionQueue::Add([=]()
        {
            Destroy();
        });
    }

    void VulkanTexture::SetData([[maybe_unused]] const void* data)
    {
    }

    void VulkanTexture::Destroy()
    {
        if (m_Sampler)
        {
            vkDestroySampler(static_cast<VkDevice>(m_Device->GetLogical()), m_Sampler, VK_NULL_HANDLE);
            m_Sampler = VK_NULL_HANDLE;
        }

        if (m_Image)
            m_Image->Destroy();

        if (m_StagingBuffer)
        {
            vmaDestroyBuffer(VulkanAllocator::Get(), m_StagingBuffer, m_StagingAllocation);
            m_StagingBuffer = VK_NULL_HANDLE;
            m_StagingAllocation = VK_NULL_HANDLE;
        }
    }
    VkFilter VulkanTexture::ToVkFilter(SampleFilter filter)
    {
        switch (filter)
        {
            case SampleFilter::Linear:    return VK_FILTER_LINEAR;
            case SampleFilter::Nearest:   return VK_FILTER_NEAREST;
            case SampleFilter::Undefined: return VK_FILTER_MAX_ENUM;
            default:                      return VK_FILTER_MAX_ENUM;
        }
    }

    VkSamplerAddressMode VulkanTexture::ToVkAddressMode(TextureWrapMode mode)
    {
        switch (mode)
        {
            case TextureWrapMode::Repeat:         return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case TextureWrapMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case TextureWrapMode::ClampToEdge:    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case TextureWrapMode::ClampToBorder:  return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            default:                              return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
        }
    }
}