#include "Buffer.h"

#include "Renderer.h"
#include "OpenGL/OpenGLBuffer.h"
#include "Vulkan/VulkanBuffer.h"

namespace pxl
{
    // Static Buffer
    std::shared_ptr<Buffer> Buffer::Create(BufferUsage usage, uint32_t size, const void* data)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLBuffer>(usage, size, data);
            case RendererAPIType::Vulkan:
                auto vulkanDevice = dynamic_pointer_cast<VulkanDevice>(Renderer::GetCurrentDevice());
                if (!vulkanDevice)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Can't create VulkanBuffer, failed to retrieve VulkanDevice from renderer");
                    break;
                }

                return std::make_shared<VulkanBuffer>(vulkanDevice, usage, size, data);
        }

        return nullptr;
    }

    // Dynamic Buffer
    std::shared_ptr<Buffer> Buffer::Create(BufferUsage usage, uint32_t size)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLBuffer>(usage, size);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for Vulkan renderer api.");
                break;
        }
        
        return nullptr;
    }
}