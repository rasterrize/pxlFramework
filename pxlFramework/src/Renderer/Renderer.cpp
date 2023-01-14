#include "Renderer.h"

#include "../../src/Core/Window.h"
#include <glad/glad.h>

#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLShader.h"

#include "../Core/Application.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPI Renderer::s_RendererAPI;
    GLFWwindow* Renderer::s_WindowHandle;
    std::unique_ptr<GraphicsContext> Renderer::s_GraphicsContext;

    //std::shared_ptr<GraphicsContext> Renderer::s_GraphicsContext;
    std::shared_ptr<VertexBuffer> Renderer::s_VertexBuffer;
    std::shared_ptr<VertexArray> Renderer::s_VertexArray;
    std::shared_ptr<Shader> Renderer::s_Shader;

    void Renderer::Init(RendererAPI api)
    {   
        if (s_Enabled)
        {
            Logger::Log(LogLevel::Warn, "Can't initialize renderer, it's already initialized.");
        }

        s_WindowHandle = Window::GetNativeWindow();

        if (!s_WindowHandle)
        {
            Logger::Log(LogLevel::Error, "Can't initialize renderer, no window handle exists (window must be initialized first)");
        }

        float positions[6] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.0f, 0.5f
        };

        std::string vertexShaderSource = R"(
            #version 450 core

            layout (location = 0) in vec3 a_Position;

            void main()
            {
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

        std::string vertexShaderCamera = R"(
            #version 450 core

            layout (location = 0) in vec3 a_Position;

            uniform mat4 u_Projection;
            uniform mat4 u_View;

            void main()
            {
                gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 450 core
            
            layout (location = 0) out vec4 color;

            void main()
            {
                color = vec4(1.0, 0.2, 0.4, 1.0);
            }
        )";

        switch (api)
        {
            case RendererAPI::OpenGL:
            {
                s_GraphicsContext = std::make_unique<OpenGLContext>();
                if (!s_GraphicsContext)
                {
                    Logger::LogError("Failed to create OpenGL graphics context");
                }
                else
                {
                    Logger::LogInfo("Successfully created OpenGL graphics context");
                }

                s_VertexBuffer = std::make_shared<OpenGLVertexBuffer>(sizeof(positions), positions);
                if (!s_VertexBuffer)
                {
                    Logger::LogError("Failed to create OpenGL vertex buffer object");
                }
                else 
                {
                    Logger::LogInfo("Successfully created OpenGL vertex buffer with size: " + std::to_string(sizeof(positions)) + " bytes");
                }

                s_VertexArray = std::make_shared<OpenGLVertexArray>();
                if (!s_VertexArray)
                {
                    Logger::LogError("Failed to create OpenGL vertex array object");
                }
                else
                {
                    Logger::LogInfo("Successfully created OpenGL vertex array object");
                }

                s_Shader = std::make_shared<OpenGLShader>(vertexShaderCamera, fragmentShaderSource);
                if (!s_Shader)
                {
                    Logger::LogError("Failed to create OpenGL shader object");
                }
                else
                {
                    Logger::LogInfo("Successfully created OpenGL shader object");
                }

                // TEMP
                BufferLayout layout;
                layout.Add(2, BufferDataType::Float, false);
                s_VertexArray->SetLayout(layout);

                break;
            }
            case RendererAPI::Vulkan:
            {
                Logger::LogError("Vulkan isn't currently supported, closing application");
                Application::Get().Close();
                return;
            }
            case RendererAPI::DirectX12:
            {
                Logger::LogError("DirectX isn't currently supported, closing application");
                Application::Get().Close();
                return;
            }
        }

        s_RendererAPI = api;
        s_Enabled = true;
    }

    void Renderer::Shutdown()
    {
        s_Enabled = false;
    }

    void Renderer::Clear()
    {
        // Not api-agnostic
        if (s_Enabled)
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }

    void Renderer::SetClearColour(float r, float g, float b, float a)
    {
        // Not api-agnostic
        if (s_Enabled)
        {
            glClearColor(r, g, b, a);
        }
    }

    void Renderer::Draw()
    {
        if (s_Enabled)
        {
            s_VertexBuffer->Bind();
            s_VertexArray->Bind();
            s_Shader->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 3);
            s_GraphicsContext->SwapBuffers();
        }
    }
}