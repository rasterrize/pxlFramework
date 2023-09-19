#include "Camera.h"

#include "Renderer.h"
#include "OpenGL/OpenGLOrthographicCamera.h"
#include "OpenGL/OpenGLPerspectiveCamera.h"

namespace pxl
{
    std::vector<std::shared_ptr<Camera>> Camera::s_Cameras;

    std::shared_ptr<Camera> Camera::Create(CameraType type)
    {
        std::shared_ptr<Camera> camera;

        switch (Renderer::GetAPIType())
        {
            case RendererAPIType::None:
                Logger::LogError("Can't create camera for no renderer api");
                return nullptr;
            case RendererAPIType::OpenGL:
                switch (type)
                {
                    case CameraType::Orthographic:
                        camera = std::make_shared<OpenGLOrthographicCamera>();
                        if (camera)
                            Logger::LogInfo("OpenGL orthographic camera created");
                        break;
                    case CameraType::Perspective:
                        camera = std::make_shared<OpenGLPerspectiveCamera>();
                        if (camera)
                            Logger::LogInfo("OpenGL perspective camera created");
                        break;
                }
                break;
            case RendererAPIType::Vulkan:
                Logger::LogWarn("Vulkan Cameras not implemented");
                return nullptr;
        }

        if (camera) // technically this check is unnecessary but im gonna leave it here for now
        {
            s_Cameras.push_back(camera);
            camera->m_Handle = camera;
            return camera;
        }

        Logger::LogError("Failed to create camera");
        return nullptr;
    }

    void Camera::UpdateAll()
    {
        for (auto camera : s_Cameras)
        {
            camera->Update();
        }
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