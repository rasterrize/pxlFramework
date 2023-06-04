#include "OpenGLShader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace pxl
{
    OpenGLShader::OpenGLShader(const std::string& vertSrc, const std::string& fragSrc)
    {
        // Read our shaders into the appropriate buffers
        std::string vertexSource = vertSrc;
        std::string fragmentSource = fragSrc;

        // Create an empty vertex shader handle
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar *source = (const GLchar *)vertexSource.c_str();
        glShaderSource(vertexShader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(vertexShader);

        GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

        if(isCompiled == GL_FALSE)
        {
        	GLint maxLength = 0;
        	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        	// The maxLength includes the NULL character
        	std::vector<GLchar> infoLog(maxLength);
        	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

        	// We don't need the shader anymore.
        	glDeleteShader(vertexShader);

            Logger::LogError(infoLog.data());
            
        	return;
        }

        // Create an empty fragment shader handle
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const GLchar *)fragmentSource.c_str();
        glShaderSource(fragmentShader, 1, &source, 0);

        // Compile the fragment shader
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
        	GLint maxLength = 0;
        	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        	// The maxLength includes the NULL character
        	std::vector<GLchar> infoLog(maxLength);
        	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        	// We don't need the shader anymore.
        	glDeleteShader(fragmentShader);
        	// Either of them. Don't leak shaders.
        	glDeleteShader(vertexShader);

        	Logger::LogError(infoLog.data());

        	return;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        m_RendererID = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(m_RendererID, vertexShader);
        glAttachShader(m_RendererID, fragmentShader);

        // Link our program
        glLinkProgram(m_RendererID);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
        	GLint maxLength = 0;
        	glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

        	// The maxLength includes the NULL character
        	std::vector<GLchar> infoLog(maxLength);
        	glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

        	// We don't need the program anymore.
        	glDeleteProgram(m_RendererID);
        	// Don't leak shaders either.
        	glDeleteShader(vertexShader);
        	glDeleteShader(fragmentShader);

            Logger::LogError(infoLog.data());

        	// In this simple program, we'll just leave
        	return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_RendererID, vertexShader);
        glDetachShader(m_RendererID, fragmentShader);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind()
    {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind()
    {
        glUseProgram(0);
    }

    {

    int OpenGLShader::GetUniformLocation(const std::string& name) const
    {
        if (m_UniformCache.find(name) != m_UniformCache.end())
            return m_UniformCache[name];

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        m_UniformCache[name] = location;

        return location;
    }

    void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
    {
        int location = GetUniformLocation(name);
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(value)); // fv ~ float value, dv ~ decimal value
    }
}