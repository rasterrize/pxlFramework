#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders)
    {
        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.

        if (!(shaders.at(ShaderStage::Vertex) && shaders.at(ShaderStage::Fragment)))
        {
            PXL_LOG_ERROR(LogArea::OpenGL, "OpenGL pipelines require at least a vertex AND fragment shader");
        }

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
        // TODO: Set state such as cull mode and polygon mode

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
}