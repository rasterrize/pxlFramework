#include "OpenGLPipeline.h"

namespace pxl
{
    OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(const std::shared_ptr<Shader>& shader)
        : m_Shader(shader)
    {
    }

    void OpenGLGraphicsPipeline::Bind()
    {
        m_Shader->Bind();
    }
}