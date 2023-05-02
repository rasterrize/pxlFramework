#include "Renderer.h"

#include "../../src/Core/Window.h"
#include <glad/glad.h>

#include "../Core/Application.h"

#include "RendererAPI.h"
#include "OpenGL/OpenGLRenderer.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPIType Renderer::s_RendererAPIType;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI;
    GLFWwindow* Renderer::s_WindowHandle;
    std::unique_ptr<GraphicsContext> Renderer::s_GraphicsContext;

    void Renderer::Init(RendererAPIType api)
    {   
        if (s_Enabled)
        {
            Logger::Log(LogLevel::Warn, "Can't initialize renderer, it's already initialized.");
        }

        s_WindowHandle = Window::GetNativeWindow();

        if (!s_WindowHandle) // Should check if window is initialized instead of getting the window handle (this should be thought through when multiple windows is implemented)
        {
            Logger::Log(LogLevel::Error, "Can't initialize renderer, no window handle exists (window must be initialized first)");
        }

        switch (api)
        {
            case RendererAPIType::OpenGL:
            {
                s_GraphicsContext = std::make_unique<OpenGLContext>();
                if (!s_GraphicsContext)
                {
                    Logger::LogError("Failed to create OpenGL graphics context");
                }

                Logger::LogInfo("OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)));

                s_RendererAPI = std::make_unique<OpenGLRenderer>();
                if (!s_RendererAPI)
                    Logger::LogError("Failed to create OpenGL renderer api object");

                break;
            }
            case RendererAPIType::Vulkan:
            {
                Logger::LogError("Vulkan isn't currently supported, closing application");
                Application::Get().Close();
                return;
            }
            case RendererAPIType::DirectX12:
            {
                Logger::LogError("DirectX isn't currently supported, closing application");
                Application::Get().Close();
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
        s_GraphicsContext->SwapBuffers();
    }

    void Renderer::DrawLines(int count)
    {
        s_RendererAPI->DrawLines(count);
        s_GraphicsContext->SwapBuffers();
    }    

    void Renderer::DrawIndexed()
    {
        s_RendererAPI->DrawIndexed();
        s_GraphicsContext->SwapBuffers();
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

    // void Renderer::Submit(Mesh mesh)
    // {
    //     s_Meshes.push_back(mesh);
    // }
}