#include "Renderer.h"

#include "../../src/Core/Window.h"
#include <glad/glad.h>

#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"

#include "../Core/Application.h"

#include "../Utils/ModelLoader.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPI Renderer::s_RendererAPI;
    GLFWwindow* Renderer::s_WindowHandle;
    std::unique_ptr<GraphicsContext> Renderer::s_GraphicsContext;

    //std::shared_ptr<GraphicsContext> Renderer::s_GraphicsContext;
    std::shared_ptr<VertexBuffer> Renderer::s_VertexBuffer;
    std::shared_ptr<VertexArray> Renderer::s_VertexArray;
    std::shared_ptr<IndexBuffer> Renderer::s_IndexBuffer;
    std::shared_ptr<Shader> Renderer::s_Shader;

    void Renderer::Init(RendererAPI api)
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

        float positions[6] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.0f, 0.5f
        };

        float squarepositions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
        };

        unsigned int squareindices[] = {
            0, 1, 2,
            2, 3, 0
        };

        float cubepositions[] = {
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f
        };

        unsigned int cubeindices[] = { 
            0, 1, 2, 
            2, 3, 0, // front

            1, 5, 6,
            6, 2, 1, // right

            5, 4, 7,
            7, 6, 5, // back

            4, 0, 3,
            3, 7, 4, // left

            3, 2, 6,
            6, 7, 3, // top

            0, 1, 5,
            5, 4, 0 // bottom
        };

        std::string vertexShaderSource = R"(
            #version 450 core

            layout (location = 0) in vec3 a_Position;
            out vec3 v_Position;

            void main()
            {
                v_Position = a_Position;
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

        std::string vertexShaderOrthoCamera = R"(
            #version 450 core

            layout (location = 0) in vec3 a_Position;
            out vec3 v_Position;

            uniform mat4 u_Projection;
            uniform mat4 u_View;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_Projection * u_View vec4(a_Position, 1.0);
            }
        )";

        std::string vertexShaderCamera = R"(
            #version 450 core

            layout (location = 0) in vec3 a_Position;
            out vec3 v_Position;

            uniform mat4 u_Model;
            uniform mat4 u_Projection;
            uniform mat4 u_View;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 450 core
            
            layout (location = 0) out vec4 color;
            in vec3 v_Position;

            void main()
            {
                //color = vec4(1.0, 0.2, 0.4, 1.0);
                color = vec4(v_Position, 1.0);
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

                s_VertexBuffer = std::make_shared<OpenGLVertexBuffer>(sizeof(cubepositions), cubepositions);
                if (!s_VertexBuffer)
                {
                    Logger::LogError("Failed to create OpenGL vertex buffer object");
                }
                else 
                {
                    Logger::LogInfo("Successfully created OpenGL vertex buffer with size: " + std::to_string(sizeof(cubepositions)) + " bytes");
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

                s_IndexBuffer = std::make_shared<OpenGLIndexBuffer>(36, cubeindices);
                if (!s_IndexBuffer)
                {
                    Logger::LogError("Failed to create OpenGL index buffer object");
                }
                else
                {
                    Logger::LogInfo("Successfully created OpenGL index buffer object");
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

                glEnable(GL_DEPTH_TEST); // should this be here?

                // TEMP
                BufferLayout layout;
                layout.Add(3, BufferDataType::Float, false);
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

        s_RendererAPI = api; // could this be put into intializer list?
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
            glClear(GL_DEPTH_BUFFER_BIT);
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
            s_IndexBuffer->Bind();
            s_Shader->Bind();
            glDrawElements(GL_TRIANGLES, s_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
            //glDrawArrays(GL_TRIANGLES, 0, 24);
            s_GraphicsContext->SwapBuffers();
        }
    }
}