#include "VertexArray.h"

#include "Renderer.h"
#include "OpenGL/OpenGLVertexArray.h"

namespace pxl
{
    std::shared_ptr<VertexArray> VertexArray::Create()
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Array for no renderer api.");
                return nullptr;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLVertexArray>();
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Vertex Array for Vulkan renderer api.");
                return nullptr;
        }
        
        return nullptr;
    }
}