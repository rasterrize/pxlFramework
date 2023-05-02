#include "OpenGLPerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Renderer.h"
#include "../../Core/Window.h"

namespace pxl
{
    OpenGLPerspectiveCamera::OpenGLPerspectiveCamera()
    : BaseCamera(CameraType::Perspective, 0.001f, 1000.0f), m_FOV(45.0f)
    {
        Logger::LogInfo("OpenGL perspective camera created");

        m_ProjectionMatrix = glm::perspective(m_FOV, Window::GetAspectRatio(), m_NearClip, m_FarClip);
    }

    void OpenGLPerspectiveCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position); // add rotation with quaternions

        m_ViewMatrix = glm::inverse(transform);

        auto shader = Renderer::GetShader();
        shader->Bind();
        shader->SetUniformMat4("u_Model", m_ModelMatrix);
        shader->SetUniformMat4("u_View", m_ViewMatrix);
        shader->SetUniformMat4("u_Projection", m_ProjectionMatrix);
    }

    void OpenGLPerspectiveCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::perspective(m_FOV, Window::GetAspectRatio(), m_NearClip, m_FarClip);
    }
}