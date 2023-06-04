#include "OpenGLPerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Renderer.h"
#include "../../Core/Window.h"

namespace pxl
{
    OpenGLPerspectiveCamera::OpenGLPerspectiveCamera()
    : BaseCamera(CameraType::Perspective, 0.001f, 1000.0f), m_FOV(45.0f)
    {
        m_ProjectionMatrix = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
    }

    void OpenGLPerspectiveCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);

        auto shader = Renderer::GetShader();
        if (shader)
        {
            shader->Bind();
            shader->SetUniformMat4("u_View", m_ViewMatrix);
            shader->SetUniformMat4("u_Projection", m_ProjectionMatrix);
        }
    }

    void OpenGLPerspectiveCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
    }
}