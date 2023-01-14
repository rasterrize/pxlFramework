#include "OpenGLPerspectiveCamera.h"
#include "../Renderer.h"

#include <glm/glm.hpp>

namespace pxl
{
    OpenGLPerspectiveCamera::OpenGLPerspectiveCamera()
    {
        Logger::LogInfo("OpenGL perspective camera created");
        //m_ProjectionMatrix = glm::perspective();
        m_ViewMatrix = glm::mat4(1.0f);
    }

    void OpenGLPerspectiveCamera::Update()
    {
        std::shared_ptr<Shader> shader = Renderer::GetShader();
        shader->SetUniformMat4("u_Projection", m_ProjectionMatrix);
        shader->SetUniformMat4("u_View", m_ViewMatrix);
    }
}