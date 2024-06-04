#include "OpenGLPipeline.h"

namespace pxl
{
    void OpenGLGraphicsPipeline::Bind()
    {
        m_Shader->Bind();
    }
}