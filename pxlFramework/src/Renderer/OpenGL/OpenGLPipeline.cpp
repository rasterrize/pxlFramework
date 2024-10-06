#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders)
        : m_PolygonMode(ToGLPolygonMode(specs.PolygonMode)), m_CullMode(ToGLCullMode(specs.CullMode))
    {
        if (!(shaders.at(ShaderStage::Vertex) && shaders.at(ShaderStage::Fragment)))
        {
            PXL_LOG_ERROR(LogArea::OpenGL, "OpenGL pipelines require at least a vertex AND fragment shader");
        }

        if (m_CullMode == GL_INVALID_ENUM)
            m_CullingEnabled = false;

        m_ShaderProgramID = glCreateProgram();
        auto vertexShaderID = static_pointer_cast<OpenGLShader>(shaders.at(ShaderStage::Vertex))->GetID();
        auto fragmentShaderID = static_pointer_cast<OpenGLShader>(shaders.at(ShaderStage::Fragment))->GetID();

        // Attach our shaders to our program
        glAttachShader(m_ShaderProgramID, vertexShaderID);
        glAttachShader(m_ShaderProgramID, fragmentShaderID);

        // Link our program
        glLinkProgram(m_ShaderProgramID);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgramID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ShaderProgramID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_ShaderProgramID);
            // Don't leak shaders either.
            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);

            PXL_LOG_ERROR(LogArea::OpenGL, infoLog.data());

            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_ShaderProgramID, vertexShaderID);
        glDetachShader(m_ShaderProgramID, fragmentShaderID);
    }

    void OpenGLGraphicsPipeline::Bind()
    {
        glPolygonMode(GL_FRONT_AND_BACK, m_PolygonMode);

        m_CullingEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        glCullFace(m_CullMode);

        glFrontFace(m_FrontFace);

        glUseProgram(m_ShaderProgramID);
    }

    void OpenGLGraphicsPipeline::Unbind()
    {
        glUseProgram(0);
    }

    void OpenGLGraphicsPipeline::SetUniformData(const std::string& name, UniformDataType type, const void* data)
    {
        switch (type)
        {
            case UniformDataType::Float:
                PXL_LOG_ERROR(LogArea::OpenGL, "Float for OpenGL uniforms isn't supported atm");
                return;
            case UniformDataType::Mat4:
                glUniformMatrix4fv(GetUniformLocation(name), 1, false, (const GLfloat*)data); // fv ~ float value, dv ~ decimal value
                return;
            case UniformDataType::Int:
                //glUniform1i(GetUniformLocation(name), (GLint)data);
                PXL_LOG_ERROR(LogArea::OpenGL, "Int for OpenGL uniforms isn't supported atm");
                return;
            case UniformDataType::IntArray:
                PXL_LOG_ERROR(LogArea::OpenGL, "IntArray requires a count to upload as uniform");
                return;
        }

        PXL_LOG_ERROR(LogArea::OpenGL, "Invalid UniformDataType");
    }

    void OpenGLGraphicsPipeline::SetUniformData(const std::string& name, UniformDataType type, uint32_t count, const void* data)
    {
        switch (type)
        {
            case UniformDataType::IntArray:
                glUniform1iv(GetUniformLocation(name), count, (const GLint*)data);
                return;
        }

        PXL_LOG_ERROR(LogArea::OpenGL, "Invalid UniformDataType");
    }

    int OpenGLGraphicsPipeline::GetUniformLocation(const std::string& name) const
    {
        if (m_UniformCache.find(name) != m_UniformCache.end()) // Could this be .at() or .contains() ??
            return m_UniformCache[name];

        int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
        m_UniformCache[name] = location;

        return location;
    }

    GLenum OpenGLGraphicsPipeline::ToGLCullMode(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None:  return GL_INVALID_ENUM; // OpenGL requires disabling culling instead of passing a cull mode to glCullFace
            case CullMode::Front: return GL_FRONT;
            case CullMode::Back:  return GL_BACK;
        }

        return GL_INVALID_ENUM;
    }

    GLenum OpenGLGraphicsPipeline::ToGLPolygonMode(PolygonMode mode)
    {
        switch (mode)
        {
            case PolygonMode::Fill:  return GL_FILL;
            case PolygonMode::Line:  return GL_LINE;
            case PolygonMode::Point: return GL_POINT;
        }

        return GL_INVALID_ENUM;
    }

    GLenum OpenGLGraphicsPipeline::ToGLFrontFace(FrontFace face)
    {
        switch (face)
        {
            case FrontFace::CW:  return GL_CW;
            case FrontFace::CCW: return GL_CCW;
        }

        return GL_INVALID_ENUM;
    }
}