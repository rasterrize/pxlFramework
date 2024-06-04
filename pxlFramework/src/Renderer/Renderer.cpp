#include "Renderer.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "../Core/Platform.h"
#include "OpenGL/OpenGLRenderer.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanContext.h"
#include "../Utils/Fileloader.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPIType Renderer::s_RendererAPIType = RendererAPIType::None;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    std::shared_ptr<GraphicsContext> Renderer::s_ContextHandle = nullptr;

    float Renderer::s_FPS = 0.0f;
    uint32_t Renderer::s_FrameCount = 0;
    float Renderer::s_TimeAtLastFrame = 0.0f;

    constexpr uint32_t s_MaxQuadCount = 10000;
    constexpr uint32_t s_MaxQuadVertexCount = s_MaxQuadCount * 4;
    constexpr uint32_t s_MaxQuadIndexCount = s_MaxQuadCount * 6;

    constexpr uint32_t s_MaxCubeCount = 10000;
    constexpr uint32_t s_MaxCubeVertexCount = s_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved
    constexpr uint32_t s_MaxCubeIndexCount = s_MaxCubeCount * 36;

    constexpr uint32_t s_MaxLineCount = 10000;
    constexpr uint32_t s_MaxLineVertexCount = s_MaxLineCount * 2; // textures break on 8 vertex cubes, need to look into how this can be solved

    constexpr uint32_t s_MaxTextureUnits = 32; // TODO: I believe 16 is the minimum value for this (on computers) but this should be determined by a RenderCapabilities thing

    // --- Most objects below this comment are no longer static as they can cause significantly long compile times ---

    // Static Quad Data
    std::shared_ptr<Buffer> s_StaticQuadVBO = nullptr;
    std::shared_ptr<VertexArray> s_StaticQuadVAO = nullptr; // TODO: remove this? it feels unnecessary when the buffer layout is the same.
    std::function<void()> s_StaticQuadBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO, IBO for Vulkan
    std::vector<QuadVertex> s_StaticQuadVertices(s_MaxQuadVertexCount);
    uint32_t s_StaticQuadCount = 0;

    // Dynamic Quad Data
    std::shared_ptr<Buffer> s_QuadVBO = nullptr;
    std::shared_ptr<Buffer> s_QuadIBO = nullptr;
    std::function<void()> s_QuadBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO, IBO for Vulkan
    std::shared_ptr<Shader> s_QuadShader = nullptr;
    std::shared_ptr<GraphicsPipeline> s_QuadPipeline = nullptr;
    std::vector<QuadVertex> s_QuadVertices(s_MaxQuadVertexCount);
    std::vector<uint32_t> s_QuadIndices(s_MaxQuadIndexCount); // NOTE: currently also used by static quads
    uint32_t s_QuadCount = 0;
    std::shared_ptr<Camera> Renderer::s_QuadsCamera = nullptr;

    // Cube Data
    // static std::array<CubeVertex, s_MaxCubeVertexCount> s_CubeVertices = {};
    // static std::array<uint32_t, s_MaxCubeIndexCount> s_CubeIndices = {};
    // static size_t s_CubeVertexCount = 0;
    // static size_t s_CubeIndexCount = 0;

    // Line Data
    // static std::array<LineVertex, s_MaxLineVertexCount> s_LineVertices = {};
    // static size_t s_LineVertexCount = 0;

    // Mesh Data
    // static std::vector<std::shared_ptr<Mesh>> Renderer::s_Meshes;

    // Texture Data
    uint32_t s_TextureUnitIndex = 0;
    std::array<std::shared_ptr<Texture2D>, s_MaxTextureUnits> s_TextureSlots;

    // For OpenGL
    std::shared_ptr<VertexArray> s_QuadVAO;
    std::shared_ptr<VertexArray> s_CubeVAO;
    std::shared_ptr<VertexArray> s_LineVAO;
    std::shared_ptr<VertexArray> s_MeshVAO;
    
    Renderer::Statistics Renderer::s_Stats;

    void Renderer::Init(const std::shared_ptr<Window>& window)
    {   
        PXL_PROFILE_SCOPE;
        
        if (s_Enabled)
        {
            PXL_LOG_WARN(LogArea::Renderer, "Renderer already initialized");
            // TODO: recreate all resources used for the renderer if the renderer api has changed
        }

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
                    PXL_LOG_ERROR(LogArea::Renderer, "Failed to create OpenGL renderer api object"); // TODO: switch to assert

                break;
            case RendererAPIType::Vulkan:
                auto vulkanContext = dynamic_pointer_cast<VulkanGraphicsContext>(window->GetGraphicsContext());
                PXL_ASSERT(vulkanContext);

                s_RendererAPI = std::make_unique<VulkanRenderer>(vulkanContext);
                PXL_ASSERT(s_RendererAPI);
                    
                break;
        }

        {
            // Prepare Quad Index Data
            uint32_t offset = 0;
            for (size_t i = 0; i < s_MaxQuadIndexCount; i += 6)
            {
                s_QuadIndices[i + 0] = 0 + offset;
                s_QuadIndices[i + 1] = 1 + offset;
                s_QuadIndices[i + 2] = 2 + offset;

                s_QuadIndices[i + 3] = 2 + offset;
                s_QuadIndices[i + 4] = 3 + offset;
                s_QuadIndices[i + 5] = 0 + offset;

                offset += 4;
            }

        }

        {
            auto quadBufferLayout = QuadVertex::GetLayout();

            // Prepare Quad Buffers
            s_QuadVBO = Buffer::Create(BufferUsage::Vertex, s_MaxQuadVertexCount * sizeof(QuadVertex));
            s_QuadIBO = Buffer::Create(BufferUsage::Index, s_MaxQuadIndexCount * sizeof(uint32_t), s_QuadIndices.data());
            s_StaticQuadVBO = Buffer::Create(BufferUsage::Vertex, s_MaxQuadVertexCount * sizeof(QuadVertex));

            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_QuadVAO = VertexArray::Create();
                s_QuadVAO->AddVertexBuffer(s_QuadVBO, quadBufferLayout);
                s_QuadVAO->SetIndexBuffer(s_QuadIBO);

                s_StaticQuadVAO = VertexArray::Create();
                s_StaticQuadVAO->AddVertexBuffer(s_StaticQuadVBO, quadBufferLayout);
                s_StaticQuadVAO->SetIndexBuffer(s_QuadIBO); // Use the quad index buffer since it's also static

                s_QuadBindFunc = []() {
                    s_QuadVAO->Bind();
                    s_QuadShader->Bind();
                };

                s_StaticQuadBindFunc = []() {
                    s_StaticQuadVAO->Bind();
                    s_QuadShader->Bind();
                };

                s_QuadShader = pxl::FileLoader::LoadGLSLShader("resources/shaders/quad_coloured.vert", "resources/shaders/quad_coloured.frag");
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_QuadBindFunc = []() {
                    s_QuadVBO->Bind();
                    s_QuadIBO->Bind();
                    s_QuadPipeline->Bind(); // TODO: remove this once opengl pipelines are completely used
                };

                s_StaticQuadBindFunc = []() {
                    s_StaticQuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                auto vertBin = FileLoader::LoadSPIRV("resources/shaders/compiled/vert.spv");
                auto fragBin = FileLoader::LoadSPIRV("resources/shaders/compiled/frag.spv");
                // auto quadVertShader = Shader::Create(ShaderStage::Vertex, vertBin);
                // auto quadFragShader = Shader::Create(ShaderStage::Fragment, fragBin);

                std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;
                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertBin);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragBin);

                pxl::UniformLayout quadUniformLayout;
                quadUniformLayout.Add({ "vpPC", pxl::BufferDataType::Mat4, true, pxl::ShaderStage::Vertex });

                s_QuadPipeline = GraphicsPipeline::Create(shaders, quadBufferLayout, quadUniformLayout);
            }
        }
        // {
        //     // Prepare Cube Index Buffer
        //
        //     uint32_t offset = 0;
        //     for (size_t i = 0; i < MAX_CUBE_INDEX_COUNT; i += 6) // all this assumes the cube is made of 6 quads
        //     {
        //         s_CubeIndices[i + 0] = 0 + offset;
        //         s_CubeIndices[i + 1] = 1 + offset;
        //         s_CubeIndices[i + 2] = 2 + offset;
        //
        //         s_CubeIndices[i + 3] = 2 + offset;
        //         s_CubeIndices[i + 4] = 3 + offset;
        //         s_CubeIndices[i + 5] = 0 + offset;
        //
        //         s_CubeIndexCount += 6;
        //
        //         offset += 4;
        //     }
        // }

        #if CUBESLINESMODELS

        // {
        //     // Prepare Cube VAO, VBO, IBO
        //     s_CubeVAO = std::make_shared<OpenGLVertexArray>(); // not api agnostic
        //     auto cubeVBO = std::make_shared<OpenGLVertexBuffer>((uint32_t)(MAX_CUBE_VERTEX_COUNT * sizeof(TriangleVertex)));
        //     auto cubeIBO = std::make_shared<OpenGLIndexBuffer>((uint32_t)MAX_CUBE_INDEX_COUNT, &s_CubeIndices);

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
        //     auto lineVBO = std::make_shared<OpenGLVertexBuffer>((uint32_t)(MAX_LINE_VERTEX_COUNT * sizeof(LineVertex)));

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

        #endif

        PXL_LOG_INFO(LogArea::Renderer, "Finished preparing renderer");

        s_Enabled = true;
    }

    void Renderer::Shutdown()
    {
        s_Enabled = false;

        // Delete vulkan objects before RAII deletes them in the wrong order
        if (s_RendererAPIType == RendererAPIType::Vulkan)
        {
            s_ContextHandle->GetDevice()->WaitIdle();
            VulkanDeletionQueue::Flush();
        }
    }

    void Renderer::Clear()
    {
        PXL_PROFILE_SCOPE;
        
        s_RendererAPI->Clear();
    }

    void Renderer::SetClearColour(const glm::vec4& colour)
    {
        s_RendererAPI->SetClearColour(colour);
    }

    // void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& camera)
    // {
    //     shader->Bind();
    //     shader->SetUniformMat4("u_VP", camera->GetViewProjectionMatrix());
    // }

    // void Renderer::Submit(const std::shared_ptr<GraphicsPipeline>& pipeline)
    // {
    //     //auto vp = s_QuadsCamera->GetViewProjectionMatrix();
    //     pipeline->Bind();
    //     // TODO: set uniform variables
    // }

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return;

        s_RendererAPI->Begin();

        // Ensure these are set before batch rendering
        //s_CubeVertexCount = 0;
        //s_LineVertexCount = 0;
    }

    void Renderer::End()
    {
        PXL_PROFILE_SCOPE;

        if (!s_Enabled)
            return;
        
        Flush();

        s_RendererAPI->End();
    }

    void Renderer::AddStaticQuad(const glm::vec3& position)
    {
        PXL_PROFILE_SCOPE;
        
        constexpr auto colour = glm::vec4(1.0f);
        const auto vertexCount = s_StaticQuadCount * 4;

        s_StaticQuadVertices[vertexCount + 0] = {{ position.x, position.y, position.z }, colour, { 0.0f, 0.0f }};
        s_StaticQuadVertices[vertexCount + 1] = {{ position.x + 1.0f, position.y , position.z }, colour, { 1.0f, 0.0f }};
        s_StaticQuadVertices[vertexCount + 2] = {{ position.x + 1.0f, position.y + 1.0f, position.z }, colour, { 1.0f, 1.0f }};
        s_StaticQuadVertices[vertexCount + 3] = {{ position.x, position.y + 1.0f, position.z }, colour, { 0.0f, 1.0f }};

        s_StaticQuadCount++;
    }

    void Renderer::StaticGeometryReady()
    {
        PXL_PROFILE_SCOPE;
        
        s_StaticQuadVBO = Buffer::Create(BufferUsage::Vertex, s_StaticQuadCount * 4 * sizeof(QuadVertex), s_StaticQuadVertices.data());

        auto layout = QuadVertex::GetLayout();

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            s_StaticQuadVAO->AddVertexBuffer(s_StaticQuadVBO, layout);
            s_StaticQuadVAO->SetIndexBuffer(s_QuadIBO);
        }
    }

    void Renderer::DrawStaticQuads()
    {
        PXL_PROFILE_SCOPE;
        
        s_StaticQuadBindFunc();

        s_RendererAPI->DrawIndexed(s_StaticQuadCount * 6); // TODO: use a cached s_StaticQuadIndexCount variable so it doesnt have to math everytime
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;
        
        if (s_QuadCount >= s_MaxQuadVertexCount)
            Flush();

        const float texIndex = -1.0f;
        const auto vertexCount = s_QuadCount * 4;
        
        s_QuadVertices[vertexCount + 0] = {{ position.x, position.y, position.z }, colour, { 0.0f, 0.0f }, texIndex };
        s_QuadVertices[vertexCount + 1] = {{ position.x + scale.x, position.y, position.z }, colour, { 1.0f, 0.0f }, texIndex };
        s_QuadVertices[vertexCount + 2] = {{ position.x + scale.x, position.y + scale.y, position.z }, colour, { 1.0f, 1.0f }, texIndex };
        s_QuadVertices[vertexCount + 3] = {{ position.x, position.y + scale.y, position.z }, colour, { 0.0f, 1.0f }, texIndex };

        s_QuadCount++;
        
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    }

    void Renderer::AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::shared_ptr<Texture2D>& texture)
    {
        PXL_PROFILE_SCOPE;
        
        if (s_QuadCount >= s_MaxQuadCount)
            Flush();

        if (s_TextureUnitIndex >= s_MaxTextureUnits)
            Flush();

        float textureIndex = -1.0f;

        for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
        {
            if (texture == s_TextureSlots[i])
            {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == -1.0f)
        {
            textureIndex = static_cast<float>(s_TextureUnitIndex);
			s_TextureSlots[s_TextureUnitIndex] = texture;
			s_TextureUnitIndex++;
        }

        constexpr auto vertexColor = glm::vec4(1.0f);
        const auto vertexCount = s_QuadCount * 4;
        
        s_QuadVertices[vertexCount + 0] = {{ position.x, position.y + scale.y, position.z }, vertexColor, { 0.0f, 1.0f }, textureIndex };
        s_QuadVertices[vertexCount + 1] = {{ position.x, position.y, position.z }, vertexColor, { 0.0f, 0.0f }, textureIndex };
        s_QuadVertices[vertexCount + 2] = {{ position.x + scale.x, position.y, position.z }, vertexColor, { 1.0f, 0.0f }, textureIndex };
        s_QuadVertices[vertexCount + 3] = {{ position.x + scale.x, position.y + scale.y, position.z }, vertexColor, { 1.0f, 1.0f }, textureIndex };

        s_QuadCount++;
        
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    }

    // void Renderer::AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    // {
    //     if (s_QuadVertexCount >= MAX_QUAD_VERTEX_COUNT)
    //     {
    //         EndQuadBatch();
    //         DrawQuads();
    //         StartQuadBatch();
    //     }
    //
    //     s_QuadVertices[s_QuadVertexCount + 0] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 0.0f, 0.0f }};
    //     s_QuadVertices[s_QuadVertexCount + 1] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 1.0f, 0.0f }};
    //     s_QuadVertices[s_QuadVertexCount + 2] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 1.0f, 1.0f }};
    //     s_QuadVertices[s_QuadVertexCount + 3] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z }, glm::vec4(1.0f), { 0.0f, 1.0f }};
    //
    //     s_QuadVertexCount += 4;
    //    
    //     s_Stats.VertexCount += 4;
    //     s_Stats.IndexCount += 6;
    // }

    // void Renderer::AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    // {
        // if (s_CubeVertexCount >= MAX_CUBE_VERTEX_COUNT)
        // {
        //     Flush();
        // }

        // // Front
        // s_CubeVertices[s_CubeVertexCount + 0] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 1] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 2] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        // s_CubeVertices[s_CubeVertexCount + 3] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // // Back
        // s_CubeVertices[s_CubeVertexCount + 4] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 5] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 6] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};
        // s_CubeVertices[s_CubeVertexCount + 7] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};

        // // Left
        // s_CubeVertices[s_CubeVertexCount + 8]  = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 9]  = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 10] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        // s_CubeVertices[s_CubeVertexCount + 11] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // // Top
        // s_CubeVertices[s_CubeVertexCount + 12] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 13] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 14] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        // s_CubeVertices[s_CubeVertexCount + 15] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // // Right
        // s_CubeVertices[s_CubeVertexCount + 16] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 17] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 18] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        // s_CubeVertices[s_CubeVertexCount + 19] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // // Bottom
        // s_CubeVertices[s_CubeVertexCount + 20] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 21] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        // s_CubeVertices[s_CubeVertexCount + 22] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        // s_CubeVertices[s_CubeVertexCount + 23] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // s_CubeVertexCount += 24;
        
    //     s_Stats.QuadVertexCount += 24; // TODO: probably should be CubeVertexCount
    //     s_Stats.QuadIndexCount += 36; // based on number of cubes, not actual indices in the index buffer
    // }

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

    // void Renderer::AddLine(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    // {
        // if (s_LineVertexCount >= MAX_LINE_VERTEX_COUNT)
        // {
        //     Flush();
        // }
        
        // s_LineVertices[s_LineVertexCount + 0] = {{ position1.x * scale.x, position1.y * scale.y, position1.z * scale.z }, colour };
        // s_LineVertices[s_LineVertexCount + 1] = {{ position2.x * scale.x, position2.y * scale.y, position2.z * scale.z }, colour };

        // s_LineVertexCount += 2;

        // s_Stats.LineVertexCount += 2;
    //}

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
        PXL_PROFILE_SCOPE;
        
        // Bind textures
        // for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
        //         s_TextureSlots[i]->Bind(i);
        
        // s_TextureUnitIndex = 0;

        // Flush quads if necessary
        if (s_QuadCount > 0)
        {
            s_QuadVBO->SetData(s_QuadCount * 4 * sizeof(QuadVertex), s_QuadVertices.data()); // THIS TAKES SIZE IN BYTES

            s_QuadBindFunc(); // Bind quad objects

            // int32_t samplers[s_MaxTextureUnits];
            // for (int32_t i = 0; i < s_MaxTextureUnits; i++)
            //     samplers[i] = i;

            //s_QuadShader->SetUniformIntArray("u_Textures", samplers, s_MaxTextureUnits);

            // TEMP: set uniforms based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                if (s_QuadShader)
                    s_QuadShader->SetUniformMat4("u_VP", s_QuadsCamera->GetViewProjectionMatrix());
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                std::unordered_map<std::string, const void*> pcData;
                auto vp = s_QuadsCamera->GetViewProjectionMatrix();
                pcData["vpPC"] = &vp;
                s_QuadPipeline->SetPushConstantData(pcData);
            }

            s_RendererAPI->DrawIndexed(s_QuadCount * 6);
            s_Stats.DrawCalls++;

            s_QuadCount = 0;
        }

        if (s_StaticQuadCount > 0)
        {

        }

        // if (s_CubeVertexCount > 0)
        // {
        //     s_CubeVAO->GetVertexBuffer()->SetData(s_CubeVertexCount * sizeof(TriangleVertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES
        //     s_CubeVAO->Bind();
        //     s_RendererAPI->DrawIndexed(s_QuadIndexCount);
        //     s_Stats.DrawCalls++;
        
        //     s_CubeVertexCount = 0;
        // }

        // if (s_LineVertexCount > 0)
        // {
        //     s_LineVAO->GetVertexBuffer()->SetData(s_LineVertexCount * sizeof(LineVertex), s_LineVertices.data());
        //     s_LineVAO->Bind();
        //     s_RendererAPI->DrawLines(s_LineVertexCount);
        //     s_Stats.DrawCalls++;
        
        //     s_LineVertexCount = 0;
        // }
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