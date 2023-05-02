#include "OpenGLOrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Renderer.h"
#include "../../Core/Window.h"

namespace pxl
{
    OpenGLOrthographicCamera::OpenGLOrthographicCamera() 
    : BaseCamera(CameraType::Orthographic, -1.0f, 1.0f)
    {
        m_ProjectionMatrix = glm::ortho(-1.6f, 1.6f, -0.9f, 0.9f, m_NearClip, m_FarClip);
    }

    void OpenGLOrthographicCamera::Update()
    {
        glm::mat4 transform = 
          glm::translate(glm::mat4(1.0f), m_Position)
        * glm::rotate(glm::mat4(1.0f), m_Rotation.x, glm::vec3(0, 0, 1)); // add rotation with quaternions

        m_ViewMatrix = glm::inverse(transform);

        auto shader = Renderer::GetShader();
        shader->Bind();
        shader->SetUniformMat4("u_Model", m_ModelMatrix);
        shader->SetUniformMat4("u_View", m_ViewMatrix);
        shader->SetUniformMat4("u_Projection", m_ProjectionMatrix);
    }

    void OpenGLOrthographicCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::ortho(-1.6f, 1.6f, -0.9f, 0.9f, m_NearClip, m_FarClip);
    }
}