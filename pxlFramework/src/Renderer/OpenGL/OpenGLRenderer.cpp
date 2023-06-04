#include "OpenGLRenderer.h"

namespace pxl
{
    OpenGLRenderer::OpenGLRenderer()
    {
        //s_VertexBuffer = std::make_shared<OpenGLVertexBuffer>(0, nullptr); 
        //s_VertexArray = std::make_shared<OpenGLVertexArray>();
        //s_IndexBuffer = std::make_shared<OpenGLIndexBuffer>(0, nullptr);

        // TODO: add checking for variable existences

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
}