#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    OrthographicCamera::OrthographicCamera(const OrthographicSettings& settings)
        : m_Settings(settings)
    {
        RecalculateProjection();
        RecalculateView();
    }

    void OrthographicCamera::RecalculateProjection()
    {
        PXL_PROFILE_SCOPE;

        m_ProjectionMatrix = glm::ortho(m_Settings.Sides.Left, m_Settings.Sides.Right, m_Settings.Sides.Bottom, m_Settings.Sides.Top, m_Settings.CameraSettings.NearClip, m_Settings.CameraSettings.FarClip);
    }

    void OrthographicCamera::RecalculateView()
    {
        PXL_PROFILE_SCOPE;

        // clang-format off
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(0, 0, 1)); // TODO: add rotation with quaternions
        // clang-format off

        m_ViewMatrix = glm::inverse(transform);
    }
}