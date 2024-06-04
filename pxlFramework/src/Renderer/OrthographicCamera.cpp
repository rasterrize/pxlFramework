#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    OrthographicCamera::OrthographicCamera(const CameraSettings& settings) 
        : Camera(settings)
    {
        RecalculateSides();
        RecalculateProjection();
    }

    void OrthographicCamera::Update()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(0, 0, 1)); // TODO: add rotation with quaternions

        m_ViewMatrix = glm::inverse(transform);
    }

    void OrthographicCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_CameraSettings.NearClip, m_CameraSettings.FarClip);
    }

    void OrthographicCamera::RecalculateSides()
    {
        m_Left = -m_CameraSettings.AspectRatio * m_Zoom;
        m_Right = m_CameraSettings.AspectRatio * m_Zoom;
        m_Bottom = -m_Zoom;
        m_Top = m_Zoom;
    }
}