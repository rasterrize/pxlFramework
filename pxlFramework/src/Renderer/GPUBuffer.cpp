#include "GPUBuffer.h"

#include "OpenGL/OpenGLBuffer.h"
#include "Renderer.h"
#include "Vulkan/VulkanBuffer.h"

namespace pxl
{
    // Static GPUBuffer
    std::shared_ptr<GPUBuffer> GPUBuffer::Create(GPUBufferUsage usage, uint32_t size, const void* data)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex GPUBuffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLBuffer>(usage, size, data);
            case RendererAPIType::Vulkan:
                auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(Renderer::GetGraphicsContext()->GetDevice());
                return std::make_shared<VulkanBuffer>(vulkanDevice, usage, size, data);
        }

        return nullptr;
    }

    // Dynamic GPUBuffer
    std::shared_ptr<GPUBuffer> GPUBuffer::Create(GPUBufferUsage usage, uint32_t size)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex GPUBuffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLBuffer>(usage, size);
            case RendererAPIType::Vulkan:
                auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(Renderer::GetGraphicsContext()->GetDevice());
                return std::make_shared<VulkanBuffer>(vulkanDevice, usage, size, nullptr);
        }

        return nullptr;
    }
}