#include "Camera.h"
#include "Renderer.h"

#include "OpenGL/OpenGLOrthographicCamera.h"
#include "OpenGL/OpenGLPerspectiveCamera.h"

namespace pxl
{
    bool Camera::s_Enabled;
    std::shared_ptr<BaseCamera> Camera::s_Camera;

    void Camera::Init(CameraType type)
    {
        if (s_Enabled)
        {
            Logger::LogWarn("Can't initialize camera, it is already initialized");
            return;
        }

        if (!Renderer::IsInitialized())
        {
            Logger::LogWarn("Can't initialize camera, renderer must be initialized first");
            return;
        }

        // if (!Renderer::GetShader())
        // {
        //     Logger::LogWarn("Can't initialize camera, Camera needs a shader");
        //     return;
        // }

        switch (Renderer::GetRendererAPIType())
        {
            case RendererAPIType::OpenGL:
            {
                switch (type)
                {
                    case CameraType::Orthographic:
                        s_Camera = std::make_shared<OpenGLOrthographicCamera>();
                        if (s_Camera)
                        {
                            Logger::LogInfo("Successfully created OpenGL orthographic camera");
                        }
                    break;
                    case CameraType::Perspective:
                        s_Camera = std::make_shared<OpenGLPerspectiveCamera>();
                        if (s_Camera)
                        {
                            Logger::LogInfo("Successfully created OpenGL perspective camera");
                        }
                    break;
                }
            }
            break;
            case RendererAPIType::Vulkan:
            {
                Logger::LogWarn("Vulkan Cameras not implemented");
                return;
            }
            break;
            case RendererAPIType::DirectX12:
            {
                Logger::LogWarn("DirectX12 Cameras not implemented");
                return;
            }
            break;
        }

        if (!s_Camera)
        {
            Logger::LogError("Failed to create camera object");
            return;
        }

        s_Enabled = true;
    }

    void Camera::Update()
    {
        if (s_Enabled)
        {
            s_Camera->Update();
        }
    }

    void Camera::Shutdown()
    {
        s_Camera = nullptr;
        s_Enabled = false;
    }
}