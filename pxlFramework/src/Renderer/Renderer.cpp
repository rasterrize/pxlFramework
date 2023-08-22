#include "Renderer.h"

#include <glad/glad.h>
#include <glm/gtx/matrix_decompose.hpp>

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

    const size_t Renderer::s_MaxVertexCount = 2000000;
    const size_t Renderer::s_MaxIndiceCount = 2000000;

    std::vector<Vertex> Renderer::s_Vertices;
    std::vector<uint32_t> Renderer::s_Indices;
    
    std::vector<Mesh> Renderer::s_Meshes;

    void Renderer::Init(std::shared_ptr<Window> window)
    {   
        if (s_Enabled)
            Logger::Log(LogLevel::Warn, "Can't initialize renderer, it's already initialized.");

        auto windowSpecs = window->GetWindowSpecs();

        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                Logger::LogError("Can't initialize renderer since window specified no renderer api");    
                return;
            case RendererAPIType::OpenGL:
                s_RendererAPI = std::make_unique<OpenGLRenderer>();

                if (!s_RendererAPI)
                    Logger::LogError("Failed to create OpenGL renderer api object");

                break;
            case RendererAPIType::Vulkan:
                Logger::LogError("Vulkan isn't currently supported");
                return;
        }

        s_Vertices.reserve(s_MaxVertexCount);
        s_Indices.reserve(s_MaxIndiceCount);

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

        if (elapsedTime > 0.05f)
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

    void Renderer::Submit(const Mesh& mesh)
    {
        s_Meshes.push_back(mesh);
    }

    void Renderer::BatchGeometry()
    {    
        int VertexCount = 0;

        for (const Mesh& mesh : s_Meshes)
        {
            auto vertices = mesh.Vertices;
            auto indices = mesh.Indices;

            glm::vec3 meshPosition;
            glm::decompose(mesh.Transform, glm::vec3(1.0f), glm::quat(), meshPosition, glm::vec3(1.0f), glm::vec4(1.0f));

            // Vertices
            Vertex batchedVertex = {{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }};
            for (const Vertex& vertex : vertices)
            {
                batchedVertex.Position = { vertex.Position[0] + meshPosition.x, vertex.Position[1] + meshPosition.y, vertex.Position[2] + meshPosition.z };
                batchedVertex.TexCoords = { vertex.TexCoords[0], vertex.TexCoords[1] };

                s_Vertices.push_back(batchedVertex);
            }

            // Indices
            for (const uint32_t& index : indices)
            {
                s_Indices.push_back(index + VertexCount);
            }

            VertexCount += mesh.Vertices.size();
        }

        // Upload data to gpu
        s_RendererAPI->GetVertexArray()->GetVertexBuffer()->SetData(s_Vertices.size() * sizeof(Vertex), s_Vertices.data()); // THIS TAKES SIZE IN BYTES
        s_RendererAPI->GetVertexArray()->GetIndexBuffer()->SetData(s_Indices.size(), s_Indices.data()); // THIS TAKES COUNT (NUMBER OF INDICES, NOT SIZE)

        // Currently, this code assumes everything is done with one batch

        s_Meshes.clear();
        s_Vertices.clear();
        s_Indices.clear();
    }

    void Renderer::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {

    }
}