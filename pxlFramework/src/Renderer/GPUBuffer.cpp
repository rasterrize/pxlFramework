#include "GPUBuffer.h"

#include "OpenGL/OpenGLBuffer.h"
#include "Renderer.h"
#include "Vulkan/VulkanBuffer.h"

namespace pxl
{
    std::shared_ptr<GPUBuffer> GPUBuffer::Create(GPUBufferUsage usage, GPUBufferDrawHint drawHint, uint32_t size, const void* data)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex GPUBuffer for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLBuffer>(usage, drawHint, size, data);
            case RendererAPIType::Vulkan:
                auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(Renderer::GetGraphicsContext()->GetDevice());
                return std::make_shared<VulkanBuffer>(vulkanDevice, usage, drawHint, size, data);
        }

        return nullptr;
    }
}