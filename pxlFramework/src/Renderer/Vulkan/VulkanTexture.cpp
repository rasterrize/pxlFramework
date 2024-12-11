#include "VulkanTexture.h"

namespace pxl
{
    VulkanTexture::VulkanTexture(const std::shared_ptr<VulkanDevice>& device, const Image& image)
    {
        m_Image = std::make_unique<VulkanImage>(device, image.Metadata.Size, VK_FORMAT_R8G8B8A8_SRGB); // TODO: unhardcode format

        // m_Image->SetData()

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
        if (m_Image)
            m_Image->Destroy();

        if (m_StagingBuffer)
        {
            vmaDestroyBuffer(VulkanAllocator::Get(), m_StagingBuffer, m_StagingAllocation);
            m_StagingBuffer = VK_NULL_HANDLE;
            m_StagingAllocation = VK_NULL_HANDLE;
        }
    }
}