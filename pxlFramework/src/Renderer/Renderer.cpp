#include "Renderer.h"

//#include <glm/gtx/matrix_decompose.hpp>

#include "Buffer.h"
#include "../Core/Platform.h"
#include "OpenGL/OpenGLRenderer.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanContext.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPIType Renderer::s_RendererAPIType = RendererAPIType::None;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    std::shared_ptr<Window> Renderer::s_WindowHandle = nullptr;
    std::shared_ptr<GraphicsContext> Renderer::s_ContextHandle = nullptr;

    float Renderer::s_FPS = 0.0f;
    uint32_t Renderer::s_FrameCount = 0;
    float Renderer::s_TimeAtLastFrame = 0.0f;

    constexpr size_t s_MaxQuadCount = 10000;
    constexpr size_t s_MaxQuadVertexCount = s_MaxQuadCount * 4;
    constexpr size_t s_MaxQuadIndexCount = s_MaxQuadCount * 6;

    constexpr size_t s_MaxCubeCount = 10000;
    constexpr size_t s_MaxCubeVertexCount = s_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved
    constexpr size_t s_MaxCubeIndexCount = s_MaxCubeCount * 36;

    constexpr size_t s_MaxLineCount = 10000;
    constexpr size_t s_MaxLineVertexCount = s_MaxLineCount * 2; // textures break on 8 vertex cubes, need to look into how this can be solved

    std::array<TriangleVertex, s_MaxQuadVertexCount> s_QuadVertices = {};
    std::array<uint32_t, s_MaxQuadIndexCount> s_QuadIndices = {};
 
    std::array<TriangleVertex, s_MaxCubeVertexCount> s_CubeVertices = {};
    std::array<uint32_t, s_MaxCubeIndexCount> s_CubeIndices = {};

    std::array<LineVertex, s_MaxLineVertexCount> s_LineVertices = {};

    // uint32_t Renderer::s_StaticQuadVertexCount = 0;
    // uint32_t Renderer::s_StaticQuadIndexCount = 0;

    size_t s_QuadVertexCount = 0;
    size_t s_QuadIndexCount = 0;

    size_t s_CubeVertexCount = 0;
    size_t s_CubeIndexCount = 0;

    size_t s_LineVertexCount = 0;

    // test
    std::shared_ptr<Buffer> Renderer::s_StaticQuadVBO;
    std::shared_ptr<VertexArray> Renderer::s_StaticQuadVAO;
    std::array<TriangleVertex, 4> s_StaticQuadVertices = {};
    uint32_t Renderer::s_StaticQuadCount = 0;
    //uint32_t Renderer::s_StaticQuadVertexCount = 0;

    // TODO: an alternative method should be used for VAOs because they are OpenGL specific
    std::shared_ptr<VertexArray> Renderer::s_QuadVAO;
    std::shared_ptr<Buffer> Renderer::s_QuadVBO;
    std::shared_ptr<Buffer> Renderer::s_QuadIBO;
    std::shared_ptr<VertexArray> Renderer::s_CubeVAO;
    std::shared_ptr<VertexArray> Renderer::s_LineVAO;
    std::shared_ptr<VertexArray> Renderer::s_MeshVAO;

    std::shared_ptr<Device> Renderer::s_Device;
    
    std::vector<std::shared_ptr<Mesh>> Renderer::s_Meshes;

    Renderer::Statistics Renderer::s_Stats;

    void Renderer::Init(const std::shared_ptr<Window>& window)
    {   
        if (s_Enabled)
        {
            PXL_LOG_WARN(LogArea::Renderer, "Renderer already initialized");
            // TODO: recreate all resources used for the renderer if the renderer api has changed
        }

        s_WindowHandle = window;
        s_ContextHandle = window->GetGraphicsContext();
        s_RendererAPIType = window->GetWindowSpecs().RendererAPI;

        switch (s_RendererAPIType)
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't initialize renderer since window specified no renderer api");    
                return;
            case RendererAPIType::OpenGL:
                s_RendererAPI = std::make_unique<OpenGLRenderer>();

                if (!s_RendererAPI)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Failed to create OpenGL renderer api object"); // need assertions
                }

                break;
            case RendererAPIType::Vulkan:
                auto vulkanContext = dynamic_pointer_cast<VulkanContext>(window->GetGraphicsContext());

                if (!vulkanContext)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Failed to retrieve graphics context from window for VulkanRenderer object");
                    return;
                }

                s_RendererAPI = std::make_unique<VulkanRenderer>(vulkanContext);

                if (!s_RendererAPI)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Failed to create Vulkan renderer api object"); // need assertions
                }

                s_Device = vulkanContext->GetDevice();

                if (!s_Device)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "s_Device was nullptr");
                }
                    
                break;
        }

        {
            // Prepare Quad Index Buffer

            uint32_t offset = 0;
            for (size_t i = 0; i < s_MaxQuadIndexCount; i += 6)
            {
                s_QuadIndices[i + 0] = 0 + offset;
                s_QuadIndices[i + 1] = 1 + offset;
                s_QuadIndices[i + 2] = 2 + offset;

                s_QuadIndices[i + 3] = 2 + offset;
                s_QuadIndices[i + 4] = 3 + offset;
                s_QuadIndices[i + 5] = 0 + offset;

                s_QuadIndexCount += 6;

                offset += 4;
            }
        }

        // {
        //     // Prepare Cube Index Buffer

        //     uint32_t offset = 0;
        //     for (size_t i = 0; i < s_MaxCubeIndexCount; i += 6) // all this assumes the cube is made of 6 quads
        //     {
        //         s_CubeIndices[i + 0] = 0 + offset;
        //         s_CubeIndices[i + 1] = 1 + offset;
        //         s_CubeIndices[i + 2] = 2 + offset;

        //         s_CubeIndices[i + 3] = 2 + offset;
        //         s_CubeIndices[i + 4] = 3 + offset;
        //         s_CubeIndices[i + 5] = 0 + offset;

        //         s_CubeIndexCount += 6;

        //         offset += 4;
        //     }
        // }

        {
            // BufferLayout layout;
            // layout.Add(BufferDataType::Float3, false); // vertex position
            // layout.Add(BufferDataType::Float4, false); // colour
            // layout.Add(BufferDataType::Float2, false); // texture coords

            // Prepare Static Quad VAO, IBO
            s_StaticQuadVAO = VertexArray::Create();
            s_QuadIBO = Buffer::Create(BufferUsage::Index, static_cast<uint32_t>(s_MaxQuadIndexCount * sizeof(uint32_t)), s_QuadIndices.data());
            s_StaticQuadVAO->SetIndexBuffer(s_QuadIBO);

            // Prepare Quad VAO, VBO, IBO
            // if (s_RendererAPIType == RendererAPIType::OpenGL)
            // {
            //     //s_QuadVBO = Buffer::Create(BufferUsage::Vertex, static_cast<uint32_t>(s_MaxQuadVertexCount * sizeof(TriangleVertex)));

            //     //s_QuadVAO = std::make_shared<OpenGLVertexArray>();

                
            //     //s_QuadVAO->SetLayout(layout);
            //     // s_QuadVAO->SetVertexBuffer(s_QuadVBO);
            //     // s_QuadVAO->SetIndexBuffer(s_QuadIBO);
            // }
            // else if (s_RendererAPIType == RendererAPIType::Vulkan)
            // {
            //     //s_QuadVBO = VertexBuffer::Create(s_Device, static_cast<uint32_t>(s_MaxQuadVertexCount * sizeof(TriangleVertex)))
            // }


            //s_QuadIBO = Buffer::Create(BufferUsage::Index, static_cast<uint32_t>(s_MaxQuadIndexCount), s_QuadIndices.data());
            //s_QuadIBO->Unbind();
        }

        // {
        //     // Prepare Cube VAO, VBO, IBO
        //     s_CubeVAO = std::make_shared<OpenGLVertexArray>(); // not api agnostic
        //     auto cubeVBO = std::make_shared<OpenGLVertexBuffer>((uint32_t)(s_MaxCubeVertexCount * sizeof(TriangleVertex)));
        //     auto cubeIBO = std::make_shared<OpenGLIndexBuffer>((uint32_t)s_MaxCubeIndexCount, &s_CubeIndices);

        //     BufferLayout layout;
        //     layout.Add(3, BufferDataType::Float, false); // vertex position
        //     layout.Add(4, BufferDataType::Float, false); // colour
        //     layout.Add(2, BufferDataType::Float, false); // texture coords

        //     s_CubeVAO->SetLayout(layout);
        //     s_CubeVAO->SetVertexBuffer(cubeVBO);
        //     s_CubeVAO->SetIndexBuffer(cubeIBO);
        // }

        // {
        //     // Prepare Line VAO, VBO
        //     s_LineVAO = std::make_shared<OpenGLVertexArray>(); // not api agnostic
        //     auto lineVBO = std::make_shared<OpenGLVertexBuffer>((uint32_t)(s_MaxLineVertexCount * sizeof(LineVertex)));

        //     BufferLayout layout;
        //     layout.Add(3, BufferDataType::Float, false); // vertex position
        //     layout.Add(4, BufferDataType::Float, false); // colour

        //     s_LineVAO->SetLayout(layout);
        //     s_LineVAO->SetVertexBuffer(lineVBO);
        // }

        // {
        //     // Prepare Mesh VAO, VBO, IBO
        //     s_MeshVAO = std::make_shared<OpenGLVertexArray>(); // not api agnostic
        //     auto meshVBO = std::make_shared<OpenGLVertexBuffer>((uint32_t)(75 * sizeof(TriangleVertex)));
        //     auto meshIBO = std::make_shared<OpenGLIndexBuffer>(426);

        //     BufferLayout layout;
        //     layout.Add(3, BufferDataType::Float, false); // vertex position
        //     layout.Add(2, BufferDataType::Float, false); // texture coords

        //     s_MeshVAO->SetLayout(layout);
        //     s_MeshVAO->SetVertexBuffer(meshVBO);
        //     s_MeshVAO->SetIndexBuffer(meshIBO);
        // }

        PXL_LOG_INFO(LogArea::Renderer, "Finished preparing renderer");

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

    void Renderer::SetClearColour(const glm::vec4& colour)
    {
        s_RendererAPI->SetClearColour(colour);
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& camera)
    {
        shader->Bind();
        //shader->SetUniformMat4("u_VP", camera->GetViewProjectionMatrix());
    }

    void Renderer::Submit(const std::shared_ptr<GraphicsPipeline>& pipeline)
    {
        pipeline->Bind();
        // TODO: set uniform variables
    }

    void Renderer::Begin()
    {
        s_RendererAPI->Begin();

        // Ensure these are set before batch rendering
        s_QuadVertexCount = 0;
        s_CubeVertexCount = 0;
        s_LineVertexCount = 0;
    }

    void Renderer::End()
    {
        s_RendererAPI->End();

        Flush();
    }

    void Renderer::AddStaticQuad(const glm::vec3& position)
    {
        s_StaticQuadVertices[s_StaticQuadCount * 4 + 0] = {{ position.x, position.y, position.z }, glm::vec4(1.0f), { 0.0f, 0.0f }};
        s_StaticQuadVertices[s_StaticQuadCount * 4 + 1] = {{ position.x + 1.0f, position.y , position.z }, glm::vec4(1.0f), { 1.0f, 0.0f }};
        s_StaticQuadVertices[s_StaticQuadCount * 4 + 2] = {{ position.x + 1.0f, position.y + 1.0f, position.z }, glm::vec4(1.0f), { 1.0f, 1.0f }};
        s_StaticQuadVertices[s_StaticQuadCount * 4 + 3] = {{ position.x, position.y + 1.0f, position.z }, glm::vec4(1.0f), { 0.0f, 1.0f }};

        s_StaticQuadCount++;
    }

    void Renderer::StaticGeometryReady()
    {
        s_StaticQuadVBO = Buffer::Create(BufferUsage::Vertex, s_StaticQuadCount * 4 * sizeof(TriangleVertex), s_StaticQuadVertices.data());

        BufferLayout layout;
        layout.Add(BufferDataType::Float3, false); // vertex position
        layout.Add(BufferDataType::Float4, false); // colour
        layout.Add(BufferDataType::Float2, false); // texture coords

        s_StaticQuadVAO->AddVertexBuffer(s_StaticQuadVBO, layout);
        //s_StaticQuadVAO->SetIndexBuffer(s_QuadIBO);
    }

    void Renderer::DrawStaticQuads()
    {
        s_StaticQuadVAO->Bind();

        s_RendererAPI->DrawIndexed(s_StaticQuadCount * 6); // use a cached s_StaticQuadIndexCount variable so it doesnt have to math everytime
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour)
    {
        if (s_QuadVertexCount >= s_MaxQuadVertexCount)
        {
            Flush();
        }
        
        s_QuadVertices[s_QuadVertexCount + 0] = {{ position.x, position.y, position.z }, colour, { 0.0f, 0.0f }};
        s_QuadVertices[s_QuadVertexCount + 1] = {{ position.x, position.y + scale.y, position.z }, colour, { 1.0f, 0.0f }};
        s_QuadVertices[s_QuadVertexCount + 2] = {{ position.x + scale.x, position.y + scale.y, position.z }, colour, { 1.0f, 1.0f }};
        s_QuadVertices[s_QuadVertexCount + 3] = {{ position.x + scale.x, position.y, position.z }, colour, { 0.0f, 1.0f }};

        s_QuadVertexCount += 4;
        
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    }

    // void Renderer::AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    // {
    //     if (s_QuadVertexCount >= s_MaxQuadVertexCount)
    //     {
    //         EndQuadBatch();
    //         DrawQuads();
    //         StartQuadBatch();
    //     }
        
    //     s_QuadVertices[s_QuadVertexCount + 0] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 0.0f, 0.0f }};
    //     s_QuadVertices[s_QuadVertexCount + 1] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 1.0f, 0.0f }};
    //     s_QuadVertices[s_QuadVertexCount + 2] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 1.0f, 1.0f }};
    //     s_QuadVertices[s_QuadVertexCount + 3] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 0.0f, 1.0f }};

    //     s_QuadVertexCount += 4;
        
    //     s_Stats.VertexCount += 4;
    //     s_Stats.IndexCount += 6;
    // }

    void Renderer::AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        if (s_CubeVertexCount >= s_MaxCubeVertexCount)
        {
            Flush();
        }

        // Front
        s_CubeVertices[s_CubeVertexCount + 0] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 1] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 2] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 3] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Back
        s_CubeVertices[s_CubeVertexCount + 4] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 5] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 6] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 7] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};

        // Left
        s_CubeVertices[s_CubeVertexCount + 8]  = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 9]  = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 10] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 11] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Top
        s_CubeVertices[s_CubeVertexCount + 12] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 13] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 14] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 15] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Right
        s_CubeVertices[s_CubeVertexCount + 16] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 17] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 18] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 19] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Bottom
        s_CubeVertices[s_CubeVertexCount + 20] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 21] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[s_CubeVertexCount + 22] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[s_CubeVertexCount + 23] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        s_CubeVertexCount += 24;
        
        s_Stats.QuadVertexCount += 24; // TODO: probably should be CubeVertexCount
        s_Stats.QuadIndexCount += 36; // based on number of cubes, not actual indices in the index buffer
    }

    // void Renderer::AddTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex)
    // {
    //     // constexpr uint32_t x = 2, y = 15;
    //     // constexpr float sheetSize = 256.0f;
    //     // constexpr float textureSize = 16.0f;
    //     // glm::vec2 texCoords[] = {
    //     //     { (textureIndex * textureSize) / sheetSize, (y * textureSize) / sheetSize },
    //     //     { ((textureIndex + 1) * textureSize) / sheetSize, (y * textureSize) / sheetSize },
    //     //     { ((textureIndex + 1) * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
    //     //     { (textureIndex * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
    //     // };
        
    //     // for (int i = 0; i < 24; i += 4)
    //     // {
    //     //     s_CubeMesh.Vertices[0 + i].TexCoords = texCoords[0]; 
    //     //     s_CubeMesh.Vertices[1 + i].TexCoords = texCoords[1]; 
    //     //     s_CubeMesh.Vertices[2 + i].TexCoords = texCoords[2]; 
    //     //     s_CubeMesh.Vertices[3 + i].TexCoords = texCoords[3]; 
    //     // }
    //     // s_CubeMesh.Translate(position.x, position.y, position.z);
    //     // s_Meshes.push_back(s_CubeMesh);
    // }

    void Renderer::AddLine(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        if (s_LineVertexCount >= s_MaxLineVertexCount)
        {
            Flush();
        }
        
        s_LineVertices[s_LineVertexCount + 0] = {{ position1.x * scale.x, position1.y * scale.y, position1.z * scale.z }, colour };
        s_LineVertices[s_LineVertexCount + 1] = {{ position2.x * scale.x, position2.y * scale.y, position2.z * scale.z }, colour };

        s_LineVertexCount += 2;

        s_Stats.LineVertexCount += 2;
    }

    // void Renderer::AddMesh(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    // {
    //     Submit(s_MeshVAO);
    //     s_RendererAPI->GetVertexArray()->GetVertexBuffer()->SetData(mesh->Vertices.size() * sizeof(TriangleVertex), mesh->Vertices.data());
    //     s_RendererAPI->GetVertexArray()->GetIndexBuffer()->SetData(mesh->Indices.size(), mesh->Indices.data());

    //     glm::mat4 transform = glm::translate(position) 
    //     * glm::rotate(glm::radians(rotation.y), glm::vec3(0, 1, 0)) 
    //     * glm::rotate(glm::radians(rotation.z), glm::vec3(0, 0, 1))
    //     * glm::rotate(glm::radians(rotation.x), glm::vec3(1, 0, 0)) // are these technically quaternion rotations?
    //     * glm::scale(scale);
        
    //     s_RendererAPI->GetShader()->SetUniformMat4("u_Transform", transform);

    //     DrawIndexed();
    // }

    void Renderer::Flush()
    {
        if (s_QuadVertexCount > 0)
        {
            s_QuadVBO->SetData(s_QuadVertexCount * sizeof(TriangleVertex), s_QuadVertices.data()); // THIS TAKES SIZE IN BYTES

            if (s_QuadVAO != nullptr) // crude way to make both opengl and vulkan to work
                s_QuadVAO->Bind();

            s_RendererAPI->DrawIndexed(s_QuadIndexCount);
            s_Stats.DrawCalls++;

            s_QuadVertexCount = 0;
        }

        if (s_CubeVertexCount > 0)
        {
            s_CubeVAO->GetVertexBuffer()->SetData(s_CubeVertexCount * sizeof(TriangleVertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES
            s_CubeVAO->Bind();
            s_RendererAPI->DrawIndexed(s_QuadIndexCount);
            s_Stats.DrawCalls++;

            s_CubeVertexCount = 0;
        }

        if (s_LineVertexCount > 0)
        {
            s_LineVAO->GetVertexBuffer()->SetData(s_LineVertexCount * sizeof(LineVertex), s_LineVertices.data());
            s_LineVAO->Bind();
            s_RendererAPI->DrawLines(s_LineVertexCount);
            s_Stats.DrawCalls++;

            s_LineVertexCount = 0;
        }
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