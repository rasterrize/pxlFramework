#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    OrthographicCamera::OrthographicCamera(const OrthographicSettings& settings)
        : m_Settings(settings)
    {
        m_Settings.UseAspectRatio ? RecalculateSidesWithAspectRatio() : RecalculateSides();
        RecalculateProjection();
    }

    void OrthographicCamera::Update()
    {
        PXL_PROFILE_SCOPE;

        // clang-format off
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
            * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(0, 0, 1)); // TODO: add rotation with quaternions
        // clang-format off

        m_ViewMatrix = glm::inverse(transform);
    }

    void OrthographicCamera::RecalculateProjection()
    {
        PXL_PROFILE_SCOPE;
        
        m_ProjectionMatrix = glm::ortho(m_Settings.Left, m_Settings.Right, m_Settings.Bottom, m_Settings.Top, m_Settings.NearClip, m_Settings.FarClip);
    }

    void OrthographicCamera::RecalculateSides()
    {
        PXL_PROFILE_SCOPE;

        m_Settings.Left = m_Settings.Left * m_Settings.Zoom;
        m_Settings.Right = m_Settings.Right * m_Settings.Zoom;
        m_Settings.Bottom = m_Settings.Bottom * m_Settings.Zoom;
        m_Settings.Top = m_Settings.Top * m_Settings.Zoom;
    }

    void OrthographicCamera::RecalculateSidesWithAspectRatio()
    {
        PXL_PROFILE_SCOPE;

        m_Settings.Left = -m_Settings.AspectRatio * m_Settings.Zoom;
        m_Settings.Right = m_Settings.AspectRatio * m_Settings.Zoom;
        m_Settings.Bottom = -m_Settings.Zoom;
        m_Settings.Top = m_Settings.Zoom;
    }
}