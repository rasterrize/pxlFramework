#include "Camera.h"

#include <glm/trigonometric.hpp>

#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"


namespace pxl
{
    std::vector<std::shared_ptr<Camera>> Camera::s_Cameras;

    std::shared_ptr<Camera> Camera::Create(const CameraSettings& settings)
    {
        std::shared_ptr<Camera> camera;
        
        switch (settings.ProjType)
        {
            case ProjectionType::None: return nullptr;
            case ProjectionType::Orthographic:
                camera = std::make_shared<OrthographicCamera>(settings);
                break;
            case ProjectionType::Perspective:
                camera = std::make_shared<PerspectiveCamera>(settings);
                break;
        }

        PXL_ASSERT(camera);

        camera->m_Handle = camera;
        s_Cameras.push_back(camera);
        return camera;
    }

    void Camera::UpdateAll()
    {
        for (auto& camera : s_Cameras)
            camera->Update();
    }

    const glm::vec3 Camera::GetForwardVector()
    {
        glm::vec3 forward;

        forward.z = -(cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        forward.x = -(sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x))); // I need to understand how this actually works
        forward.y = sin(glm::radians(m_Rotation.x));

        forward = glm::normalize(forward); // Unit Vector

        return forward;
    }

    const glm::vec3 Camera::GetUpVector()
    {
        glm::vec3 up;

        up.z = -(cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x)));
        up.x = -(sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x + 90.0f))); // I need to understand how this actually works
        up.y = sin(glm::radians(m_Rotation.x));

        up = glm::normalize(up); // Unit Vector
        return up;
    }

    const glm::vec3 Camera::GetRightVector()
    {
        glm::vec3 right;

        right.x =  cos(m_Rotation.y);
        right.z =  0;
        right.y = -sin(m_Rotation.y);

        right = glm::normalize(right); // Unit Vector

        return right;
    }
}