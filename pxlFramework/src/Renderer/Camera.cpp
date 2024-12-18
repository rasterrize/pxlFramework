#include "Camera.h"

#include <glm/trigonometric.hpp>

#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"

namespace pxl
{
    Camera::Camera()
    {
        s_Cameras.push_back(this);
    }

    Camera::~Camera()
    {
        s_Cameras.erase(std::find(s_Cameras.begin(), s_Cameras.end(), this));
    }

    void Camera::UpdateAll()
    {
        PXL_PROFILE_SCOPE;

        if (s_Cameras.empty())
            return;

        for (auto& camera : s_Cameras)
            camera->Update();
    }

    glm::vec3 Camera::GetForwardVector()
    {
        glm::vec3 forward;

        forward.z = -(cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        forward.x = -(sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x))); // I need to understand how this actually works
        forward.y = sin(glm::radians(m_Rotation.x));

        forward = glm::normalize(forward); // Unit Vector

        return forward;
    }

    glm::vec3 Camera::GetUpVector()
    {
        glm::vec3 up;

        up.z = -(cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        up.x = -(sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x + 90.0f))); // I need to understand how this actually works
        up.y = sin(glm::radians(m_Rotation.x));

        up = glm::normalize(up); // Unit Vector
        return up;
    }

    glm::vec3 Camera::GetRightVector()
    {
        glm::vec3 right;

        right.x = cos(m_Rotation.y);
        right.z = 0;
        right.y = -sin(m_Rotation.y);

        right = glm::normalize(right); // Unit Vector

        return right;
    }

    std::shared_ptr<PerspectiveCamera> Camera::CreatePerspective(const PerspectiveSettings& settings)
    {
        return std::make_shared<PerspectiveCamera>(settings);
    }

    std::shared_ptr<Camera> Camera::Create(const PerspectiveSettings& settings)
    {
        return CreatePerspective(settings);
    }

    std::shared_ptr<OrthographicCamera> Camera::CreateOrthographic(const OrthographicSettings& settings)
    {
        return std::make_shared<OrthographicCamera>(settings);
    }

    std::shared_ptr<Camera> Camera::Create(const OrthographicSettings& settings)
    {
        return CreateOrthographic(settings);
    }
}