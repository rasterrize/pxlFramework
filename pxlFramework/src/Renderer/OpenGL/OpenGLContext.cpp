#include "OpenGLContext.h"

#include <glad/glad.h>

namespace pxl
{
    [[maybe_unused]] static void GLCallback([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id,
        [[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei length, [[maybe_unused]] const GLchar* message, [[maybe_unused]] const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                PXL_LOG_ERROR(LogArea::OpenGL, message);
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                PXL_LOG_WARN(LogArea::OpenGL, message);
                break;

            case GL_DEBUG_SEVERITY_LOW:
                PXL_LOG_INFO(LogArea::OpenGL, message);
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                break;

            default:
                break;
        }
    }

    OpenGLGraphicsContext::OpenGLGraphicsContext(const std::shared_ptr<Window>& window)
        : m_GLFWWindowHandle(window->GetNativeWindow())
    {
        SetAsCurrent();
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            PXL_LOG_ERROR(LogArea::OpenGL, "Failed to initialize Glad");
        }
        else
        {
            PXL_LOG_INFO(LogArea::OpenGL, "Glad initialized - OpenGL Version: {}", (const char*)glGetString(GL_VERSION));
        }
#if PXL_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLCallback, nullptr);
#endif

        glfwSwapInterval(m_VSync);
    }

    void OpenGLGraphicsContext::Present()
    {
        PXL_PROFILE_SCOPE;

        glfwSwapBuffers(m_GLFWWindowHandle);
    }

    void OpenGLGraphicsContext::SetVSync(bool value)
    {
        value ? glfwSwapInterval(1) : glfwSwapInterval(0);
        m_VSync = value;
    }

    void OpenGLGraphicsContext::SetAsCurrent()
    {
        glfwMakeContextCurrent(m_GLFWWindowHandle);
    }

    RendererLimits OpenGLGraphicsContext::GetLimits()
    {
        int32_t maxTextureUnits;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

        return {
            .MaxTextureUnits = static_cast<uint32_t>(maxTextureUnits),
        };
    }
}