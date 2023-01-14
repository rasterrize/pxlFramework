#include "OpenGLOrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Renderer.h"

namespace pxl
{
    OpenGLOrthographicCamera::OpenGLOrthographicCamera()
    {
        Logger::LogInfo("OpenGL orthographic camera created");
        m_ProjectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    }

    void OpenGLOrthographicCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));
        m_ViewMatrix = glm::inverse(transform);

        std::shared_ptr<Shader> shader = Renderer::GetShader();
        shader->Bind();
        shader->SetUniformMat4("u_Projection", m_ProjectionMatrix);
        shader->SetUniformMat4("u_View", m_ViewMatrix);
    }
}