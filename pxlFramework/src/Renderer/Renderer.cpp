#include "Renderer.h"
#include "RendererAPI.h"
#include "OpenGL/OpenGLRenderer.h"

#include "../Core/Application.h"
#include "../../src/Core/Window.h"

#include <glad/glad.h>

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPIType Renderer::s_RendererAPIType;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI;

    void Renderer::Init(RendererAPIType api)
    {   
        if (s_Enabled)
            Logger::Log(LogLevel::Warn, "Can't initialize renderer, it's already initialized.");

        switch (api)
        {
            case RendererAPIType::OpenGL:
            {
                s_RendererAPI = std::make_unique<OpenGLRenderer>();
                if (!s_RendererAPI)
                    Logger::LogError("Failed to create OpenGL renderer api object");

                Logger::LogInfo("OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)));
                break;
            }
            case RendererAPIType::Vulkan:
            {
                Logger::LogError("Vulkan isn't currently supported");
                return;
            }
            case RendererAPIType::DirectX11:
            {
                Logger::LogError("DirectX11 isn't currently supported");
                return;
            }
            case RendererAPIType::DirectX12:
            {
                Logger::LogError("DirectX12 isn't currently supported");
                return;
            }
        }

        s_RendererAPIType = api; // could this be put into initializer list?
        s_Enabled = true;
    }

    void Renderer::Shutdown()
    {
        s_Enabled = false;
    }

    void Renderer::Clear()
    {
        s_RendererAPI->Clear();
    }

    void Renderer::SetClearColour(float r, float g, float b, float a)
    {
        s_RendererAPI->SetClearColour(r, g, b, a);
    }

    void Renderer::DrawArrays(int count)
    {
        s_RendererAPI->DrawArrays(count);
    }

    void Renderer::DrawLines(int count)
    {
        s_RendererAPI->DrawLines(count);
    }    

    void Renderer::DrawIndexed()
    {
        s_RendererAPI->DrawIndexed();
    }

    void Renderer::Submit(OpenGLVertexBuffer* vertexBuffer)
    {
        s_RendererAPI->SetVertexBuffer(vertexBuffer);
    }

    void Renderer::Submit(OpenGLVertexArray* vertexArray)
    {
        s_RendererAPI->SetVertexArray(vertexArray);
    }

    void Renderer::Submit(OpenGLIndexBuffer* indexBuffer)
    {
        s_RendererAPI->SetIndexBuffer(indexBuffer);
    }

    void Renderer::Submit(OpenGLShader* shader)
    {
        s_RendererAPI->SetShader(shader);
    }
}