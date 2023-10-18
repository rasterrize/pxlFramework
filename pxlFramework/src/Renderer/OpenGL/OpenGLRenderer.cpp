#include "OpenGLRenderer.h"

#include <glad/glad.h>

#include "../../Core/Platform.h"

namespace pxl
{
    OpenGLRenderer::OpenGLRenderer()
    {
        // could maybe check for an opengl context ???
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRenderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::SetClearColour(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }
    void OpenGLRenderer::DrawArrays(uint32_t vertexCount)
    {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    void OpenGLRenderer::DrawLines(uint32_t vertexCount)
    {
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    void OpenGLRenderer::DrawIndexed(uint32_t indexCount)
    {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
}