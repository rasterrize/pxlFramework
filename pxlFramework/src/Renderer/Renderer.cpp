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
    uint32_t Renderer::s_FrameCount = 0;
    float Renderer::s_TimeAtLastFrame = 0.0f;

    // const size_t Renderer::s_MaxQuadCount = 100000;
    // const size_t Renderer::s_MaxQuadVertexCount = s_MaxQuadCount * 4;
    // const size_t Renderer::s_MaxQuadIndexCount = s_MaxQuadCount * 6;

    // size_t Renderer::s_QuadVertexCount;
    // size_t Renderer::s_QuadIndexCount;

    // std::array<Vertex, Renderer::s_MaxQuadVertexCount> Renderer::s_QuadVertices;
    // std::array<uint32_t, Renderer::s_MaxQuadIndexCount> Renderer::s_QuadIndices;

    constexpr size_t s_MaxCubeCount = 1000;
    constexpr size_t s_MaxCubeVertexCount = s_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved
    constexpr size_t s_MaxCubeIndexCount = s_MaxCubeCount * 36;
 
    std::array<Vertex, s_MaxCubeVertexCount> s_CubeVertices {};
    std::array<uint32_t, s_MaxCubeIndexCount> s_CubeIndices {};

    size_t s_CubeVertexCount = 0;
    size_t s_CubeIndexCount = 0;
    
    std::vector<std::shared_ptr<Mesh>> Renderer::s_Meshes;

    Renderer::Statistics Renderer::s_Stats;

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

        {
            uint32_t offset = 0;
            for (size_t i = 0; i < s_MaxCubeIndexCount; i += 6) // all this assumes the cube is made of 6 quads
            {
                s_CubeIndices[i + 0] = 0 + offset;
                s_CubeIndices[i + 1] = 1 + offset;
                s_CubeIndices[i + 2] = 2 + offset;

                s_CubeIndices[i + 3] = 2 + offset;
                s_CubeIndices[i + 4] = 3 + offset;
                s_CubeIndices[i + 5] = 0 + offset;

                s_CubeIndexCount += 6;
                s_Stats.IndiceCount += 6;

                offset += 4;
            }
        }

        s_RendererAPIType = windowSpecs.RendererAPI;
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

    void Renderer::SetClearColour(const vec4& colour)
    {
        s_RendererAPI->SetClearColour(colour.r, colour.g, colour.b, colour.a);
    }

    void Renderer::DrawArrays(int count)
    {
        s_RendererAPI->DrawArrays(count);
        s_Stats.DrawCalls++;
    }

    void Renderer::DrawLines(int count)
    {
        s_RendererAPI->DrawLines(count);
        s_Stats.DrawCalls++;
    }    

    void Renderer::DrawIndexed()
    {
        s_RendererAPI->DrawIndexed();
        s_Stats.DrawCalls++;
    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
    {
        s_RendererAPI->SetVertexArray(vertexArray);
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader)
    {
        s_RendererAPI->SetShader(shader);
    }

    void Renderer::Submit(const std::shared_ptr<Mesh> mesh)
    {
        s_Meshes.push_back(mesh); // not sure if this copies the shared_ptr
    }

    void Renderer::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        if (s_CubeVertexCount >= s_MaxCubeVertexCount)
        {
            EndBatch();
            DrawIndexed();
            StartBatch();
        }

        // Front
        s_CubeVertices[s_CubeVertexCount + 0] = {{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f }, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 1] = {{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f }, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 2] = {{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f }, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 3] = {{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f }, { 0.0f, 1.0f }};

        // Back
        s_CubeVertices[s_CubeVertexCount + 4] = {{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f }, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 5] = {{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f }, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 6] = {{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f }, { 0.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 7] = {{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f }, { 1.0f, 1.0f }};

        // Left
        s_CubeVertices[s_CubeVertexCount + 8]  = {{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f }, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 9]  = {{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f }, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 10] = {{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f }, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 11] = {{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f }, { 0.0f, 1.0f }};

        // Top
        s_CubeVertices[s_CubeVertexCount + 12] = {{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f }, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 13] = {{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f }, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 14] = {{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f }, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 15] = {{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f }, { 0.0f, 1.0f }};

        // Right
        s_CubeVertices[s_CubeVertexCount + 16] = {{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f }, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 17] = {{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f }, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 18] = {{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f }, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 19] = {{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f }, { 0.0f, 1.0f }};

        // Bottom
        s_CubeVertices[s_CubeVertexCount + 20] = {{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f }, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 21] = {{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f }, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 22] = {{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f }, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 23] = {{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f }, { 0.0f, 1.0f }};

        s_CubeVertexCount += 24;
        
        s_Stats.VertexCount += 24;
        s_Stats.IndiceCount += 36; // based on number of cubes, not actual indices in the index buffer
    }

    void Renderer::DrawTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex)
    {
        // constexpr uint32_t x = 2, y = 15;
        // constexpr float sheetSize = 256.0f;
        // constexpr float textureSize = 16.0f;
        // glm::vec2 texCoords[] = {
        //     { (textureIndex * textureSize) / sheetSize, (y * textureSize) / sheetSize },
        //     { ((textureIndex + 1) * textureSize) / sheetSize, (y * textureSize) / sheetSize },
        //     { ((textureIndex + 1) * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
        //     { (textureIndex * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
        // };
        
        // for (int i = 0; i < 24; i += 4)
        // {
        //     s_CubeMesh.Vertices[0 + i].TexCoords = texCoords[0]; 
        //     s_CubeMesh.Vertices[1 + i].TexCoords = texCoords[1]; 
        //     s_CubeMesh.Vertices[2 + i].TexCoords = texCoords[2]; 
        //     s_CubeMesh.Vertices[3 + i].TexCoords = texCoords[3]; 
        // }
        // s_CubeMesh.Translate(position.x, position.y, position.z);
        // s_Meshes.push_back(s_CubeMesh);
    }

    void Renderer::StartBatch()
    {
        //s_QuadVertexCount = 0;
        s_CubeVertexCount = 0;
    }

    void Renderer::EndBatch()
    {
        s_RendererAPI->GetVertexArray()->GetVertexBuffer()->SetData(s_CubeVertexCount * sizeof(Vertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES
        s_RendererAPI->GetVertexArray()->GetIndexBuffer()->SetData(s_CubeIndexCount, s_CubeIndices.data()); // THIS TAKES COUNT (NUMBER OF INDICES, NOT SIZE)
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
}