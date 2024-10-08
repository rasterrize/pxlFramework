#include "OpenGLRenderer.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLRenderer::OpenGLRenderer()
    {
        // Set default drawing state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glEnable(GL_SCISSOR_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRenderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::SetClearColour(const glm::vec4& colour)
    {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
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

    void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLRenderer::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glScissor(x, y, width, height);
    }
}