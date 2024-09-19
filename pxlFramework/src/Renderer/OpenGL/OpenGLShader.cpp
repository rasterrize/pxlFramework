#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace pxl
{
    OpenGLShader::OpenGLShader(ShaderStage stage, const std::string& glslSrc)
        : m_ShaderStage(stage)
    {
        Compile(glslSrc);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteShader(m_RendererID);
    }

    void OpenGLShader::Reload()
    {
        // TODO: Load shader from file (hot reload)
    }

    void OpenGLShader::Compile(const std::string& glslSrc)
    {
        // Create an empty vertex shader handle
        m_RendererID = glCreateShader(ShaderStageToGLShaderStage(m_ShaderStage));

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar* source = (const GLchar*)glslSrc.c_str();
        glShaderSource(m_RendererID, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(m_RendererID);

        GLint isCompiled = 0;
        glGetShaderiv(m_RendererID, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(m_RendererID);

            PXL_LOG_ERROR(LogArea::OpenGL, infoLog.data());

            return;
        }
    }

    uint32_t OpenGLShader::ShaderStageToGLShaderStage(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:       return GL_VERTEX_SHADER;
            case ShaderStage::Fragment:     return GL_FRAGMENT_SHADER;
            case ShaderStage::Geometry:     return GL_GEOMETRY_SHADER;
            case ShaderStage::Tessellation: return GL_TESS_CONTROL_SHADER; // GL_TESS_EVALUATION_SHADER
        }

        PXL_LOG_WARN(LogArea::Vulkan, "Returning invalid OpenGL shader stage enum");
        return GL_INVALID_ENUM;
    }
}