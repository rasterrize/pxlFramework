#include "VertexBuffer.h"

#include "OpenGL/OpenGLVertexBuffer.h"
#include "Vulkan/VulkanBuffer.h"

#include "Renderer.h"

namespace pxl
{
    // Static Buffer
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size, const void* data)
    {
        switch (Renderer::GetAPIType())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLVertexBuffer>(size, data);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for vulkan without a given device");
                break;
        }

        return nullptr;
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(std::shared_ptr<Device>& device, uint32_t size, const void* data)
    {
        switch (Renderer::GetAPIType())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLVertexBuffer>(size, data);
            case RendererAPIType::Vulkan:
                return std::make_shared<VulkanBuffer>(std::dynamic_pointer_cast<VulkanDevice>(device), BufferUsage::Vertex, size, data);
        }

        return nullptr;
    }

    // Dynamic Buffer
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (Renderer::GetAPIType())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLVertexBuffer>(size);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Buffer for Vulkan renderer api.");
                break;
        }
        
        return nullptr;
    }
}