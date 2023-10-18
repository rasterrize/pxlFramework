#include "OpenGLOrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Renderer.h"
#include "../../Core/Window.h"

namespace pxl
{
    OpenGLOrthographicCamera::OpenGLOrthographicCamera() 
    : Camera(CameraType::Orthographic, { 16.0f / 9.0f, -1.0f, 1.0f })
    {
        RecalculateSides();
        RecalculateProjection();
    }

    void OpenGLOrthographicCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(0, 0, 1)); // TODO: add rotation with quaternions

        m_ViewMatrix = glm::inverse(transform);
    }

    void OpenGLOrthographicCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_CameraSettings.NearClip, m_CameraSettings.FarClip);
    }

    void OpenGLOrthographicCamera::RecalculateSides()
    {
        m_Left = -m_CameraSettings.AspectRatio * m_Zoom;
        m_Right = m_CameraSettings.AspectRatio * m_Zoom;
        m_Bottom = -m_Zoom;
        m_Top = m_Zoom;
    }
}