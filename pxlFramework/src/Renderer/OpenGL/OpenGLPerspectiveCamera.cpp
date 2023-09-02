#include "OpenGLPerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Renderer.h"
#include "../../Core/Window.h"

namespace pxl
{
    OpenGLPerspectiveCamera::OpenGLPerspectiveCamera()
    : Camera(CameraType::Perspective, { 16.0f / 9.0f, 0.001f, 1000.0f }), m_FOV(45.0f)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_CameraSettings.AspectRatio, m_CameraSettings.NearClip, m_CameraSettings.FarClip);
    }

    void OpenGLPerspectiveCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1))
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)); // are these technically quaternion rotations?

        m_ViewMatrix = glm::inverse(transform);

        auto shader = Renderer::GetShader();
        if (shader)
        {
            shader->Bind();
            shader->SetUniformMat4("u_VP", m_ProjectionMatrix * m_ViewMatrix); // MVP calculation is done on the cpu so it only is calculate once and not per vertex
        }
    }

    void OpenGLPerspectiveCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_CameraSettings.AspectRatio, m_CameraSettings.NearClip, m_CameraSettings.FarClip);
    }
}