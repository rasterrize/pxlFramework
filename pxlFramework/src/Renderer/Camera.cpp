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
                            Logger::LogInfo("Successfully created OpenGL orthographic camera");
                        break;
                    case CameraType::Perspective:
                        camera = std::make_shared<OpenGLPerspectiveCamera>();
                        if (camera)
                            Logger::LogInfo("Successfully created OpenGL perspective camera");
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
}