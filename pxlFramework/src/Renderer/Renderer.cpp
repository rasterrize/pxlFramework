#include "Renderer.h"

#include <glad/glad.h>

#include "RendererAPI.h"
#include "OpenGL/OpenGLRenderer.h"
#include "../Core/Application.h"
#include "../Core/Platform.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPIType Renderer::s_RendererAPIType;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI;

    float Renderer::s_FPS = 0.0f;
    uint16_t Renderer::s_FrameCount = 0;
    float Renderer::s_TimeAtLastFrame = 0.0f;
    float Renderer::s_LastFPS = 0.0f; 

    void Renderer::Init(std::shared_ptr<Window> window)
    {   
        if (s_Enabled)
            Logger::Log(LogLevel::Warn, "Can't initialize renderer, it's already initialized.");

        auto windowSpecs = window->GetWindowSpecs();

        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
            {
                Logger::LogError("Can't initialize renderer since window specified no renderer api");    
                return;
            }
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

        s_RendererAPIType = windowSpecs.RendererAPI;
        s_Enabled = true;
    }

    void Renderer::Shutdown()
    {
        s_Enabled = false;
    }

    void Renderer::CalculateFPS()
    {
        float currentTime = (float)Platform::GetTime();
        float elapsedTime = currentTime - s_TimeAtLastFrame;

        if (elapsedTime > 0.05)
        {
            s_FPS = s_FrameCount / elapsedTime;
            s_TimeAtLastFrame = currentTime;
            s_FrameCount = 0;
        }
    }

    void Renderer::Clear()
    {
        s_RendererAPI->Clear();
    }

    void Renderer::SetClearColour(const vec4& colour)
    {
        s_RendererAPI->SetClearColour(colour.r, colour.g, colour.b, colour.a);
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

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
    {
        s_RendererAPI->SetVertexArray(vertexArray);
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader)
    {
        s_RendererAPI->SetShader(shader);
    }

    void Renderer::Submit(const std::shared_ptr<Mesh>& mesh, float x, float y, float z)
    {
        //const int size = mesh->Vertices.size();
        auto vertices = std::array<Vertex, 100>();
        float indices[100];
        auto meshVertices = mesh->Vertices;
        auto meshIndices = mesh->Indices;

        for (int i = 0; i < meshVertices.size(); i++)
        {
            auto vertex = meshVertices[i];
            vertices[i] = {vertex.Position[0] + x, vertex.Position[1] + y, vertex.Position[2] + z};
        }

        int indiceOffset;
        for (int i = 0; i < meshIndices.size(); i++)
        {
            indices[i] = meshIndices[i];
        }

        s_RendererAPI->GetVertexArray()->GetVertexBuffer()->SetData(sizeof(vertices), &vertices);
        s_RendererAPI->GetVertexArray()->GetIndexBuffer()->SetData(100, indices);
    }
}