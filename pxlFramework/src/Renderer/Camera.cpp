#include "Camera.h"

#include <glm/trigonometric.hpp>

namespace pxl
{
    glm::vec3 Camera::GetForwardVector()
    {
        glm::vec3 forward;
        forward.z = -(cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        forward.x = -(sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        forward.y = sin(glm::radians(m_Rotation.x));

        // Convert to unit vector
        return glm::normalize(forward);
    }

    glm::vec3 Camera::GetUpVector()
    {
        glm::vec3 up;
        up.z = -(cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        up.x = -(sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x + 90.0f)));
        up.y = sin(glm::radians(m_Rotation.x));

        // Convert to unit vector
        return glm::normalize(up);
    }

    glm::vec3 Camera::GetRightVector()
    {
        glm::vec3 right;
        right.x = cos(m_Rotation.y);
        right.z = 0;
        right.y = -sin(m_Rotation.y);

        // Convert to unit vector
        return glm::normalize(right);
    }
}