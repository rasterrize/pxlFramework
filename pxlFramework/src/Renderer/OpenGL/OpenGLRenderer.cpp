#include "OpenGLRenderer.h"
#include "../../Core/Platform.h"

namespace pxl
{
    OpenGLRenderer::OpenGLRenderer()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRenderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::SetClearColour(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }
    void OpenGLRenderer::DrawArrays(int count)
    {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }

    void OpenGLRenderer::DrawLines(int count)
    {
        glDrawArrays(GL_LINE_STRIP, 0, count);
    }

    void OpenGLRenderer::DrawIndexed()
    {
        s_VertexArray->Bind();
        s_Shader->Bind();
        glDrawElements(GL_TRIANGLES, s_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRenderer::BatchGeometry()
    {
        
    }
}