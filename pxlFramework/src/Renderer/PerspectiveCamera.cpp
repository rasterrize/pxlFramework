#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    PerspectiveCamera::PerspectiveCamera(const PerspectiveSettings& settings)
        : m_Settings(settings)
    {
        RecalculateProjection();
    }

    void PerspectiveCamera::Update()
    {
        PXL_PROFILE_SCOPE;

        // clang-format off
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1))
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
        // clang-format off
        
        m_ViewMatrix = glm::inverse(transform);
    }

    void PerspectiveCamera::RecalculateProjection()
    {
        PXL_PROFILE_SCOPE;
        
        m_ProjectionMatrix = glm::perspective(glm::radians(m_Settings.FOV), m_Settings.AspectRatio, m_Settings.NearClip, m_Settings.FarClip);
    }
}