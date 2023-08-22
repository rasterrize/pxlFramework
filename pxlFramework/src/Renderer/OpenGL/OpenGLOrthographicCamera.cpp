#include "OpenGLOrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Renderer.h"
#include "../../Core/Window.h"

namespace pxl
{
    OpenGLOrthographicCamera::OpenGLOrthographicCamera() 
    : Camera(CameraType::Orthographic, {16.0f / 9.0f, 0.1f, 100.0f})
    {
        m_ProjectionMatrix = glm::ortho(-1.6f, 1.6f, -0.9f, 0.9f, m_CameraSettings.NearClip, m_CameraSettings.FarClip);
    }

    void OpenGLOrthographicCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(0, 0, 1)); // TODO: add rotation with quaternions

        m_ViewMatrix = glm::inverse(transform);

        auto shader = Renderer::GetShader();
        shader->Bind();
        shader->SetUniformMat4("u_VP", m_ProjectionMatrix * m_ViewMatrix); // MVP calculation is done on the cpu so it only is calculate once and not per vertex
    }

    void OpenGLOrthographicCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::ortho(-1.6f, 1.6f, -0.9f, 0.9f, m_CameraSettings.NearClip, m_CameraSettings.FarClip);
    }
}