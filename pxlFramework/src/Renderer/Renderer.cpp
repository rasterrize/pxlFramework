#include "Renderer.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "../Core/Platform.h"
#include "OpenGL/OpenGLRenderer.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanContext.h"
#include "../Utils/Fileloader.h"
#include "BufferLayout.h"
#include "UniformLayout.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPIType Renderer::s_RendererAPIType = RendererAPIType::None;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    std::shared_ptr<GraphicsContext> Renderer::s_ContextHandle = nullptr;

    uint32_t Renderer::s_FrameCount = 0;
    float Renderer::s_TimeAtLastFrame = 0.0f;

    constexpr uint32_t s_MaxQuadCount = 10000;
    constexpr uint32_t s_MaxQuadVertexCount = s_MaxQuadCount * 4;
    constexpr uint32_t s_MaxQuadIndexCount = s_MaxQuadCount * 6;

    constexpr uint32_t s_MaxCubeCount = 10000;
    constexpr uint32_t s_MaxCubeVertexCount = s_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved
    constexpr uint32_t s_MaxCubeIndexCount = s_MaxCubeCount * 36;

    constexpr uint32_t s_MaxLineCount = 10000;
    constexpr uint32_t s_MaxLineVertexCount = s_MaxLineCount * 2;

    constexpr uint32_t s_MaxTextureUnits = 32; // TODO: I believe 16 is the minimum value for this (on computers) but this should be determined by a RenderCapabilities thing

    // --- Most objects below this comment are no longer static as they can cause significantly long compile times ---

    // Static Quad Data
    uint32_t s_StaticQuadCount = 0;
    std::shared_ptr<Buffer> s_StaticQuadVBO = nullptr;
    std::shared_ptr<VertexArray> s_StaticQuadVAO = nullptr; // TODO: remove this? it feels unnecessary when the buffer layout is the same.
    std::function<void()> s_StaticQuadBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO/IBO for Vulkan
    std::vector<QuadVertex> s_StaticQuadVertices(s_MaxQuadVertexCount);

    // Dynamic Quad Data
    uint32_t s_QuadCount = 0;
    std::vector<QuadVertex> s_QuadVertices(s_MaxQuadVertexCount);
    std::vector<uint32_t> s_QuadIndices(s_MaxQuadIndexCount); // NOTE: currently also used by static quads
    std::shared_ptr<Buffer> s_QuadVBO = nullptr;
    std::shared_ptr<Buffer> s_QuadIBO = nullptr;
    std::function<void()> s_QuadBufferBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO/IBO for Vulkan
    std::function<void()> s_QuadUniformFunc = nullptr;
    UniformLayout s_QuadUniformLayout;
    std::shared_ptr<GraphicsPipeline> s_QuadPipeline = nullptr;
    std::shared_ptr<Camera> Renderer::s_QuadsCamera = nullptr;

    std::function<void(const std::shared_ptr<GraphicsPipeline>&, const glm::mat4& vp)> s_SetViewProjectionFunc = nullptr;

    // Cube Data
    uint32_t s_CubeCount = 0;
    std::vector<CubeVertex> s_CubeVertices(s_MaxCubeVertexCount);
    std::vector<uint32_t> s_CubeIndices(s_MaxCubeIndexCount);
    std::shared_ptr<Buffer> s_CubeVBO = nullptr;
    std::shared_ptr<Buffer> s_CubeIBO = nullptr;
    std::function<void()> s_CubeBindFunc = nullptr;
    std::shared_ptr<GraphicsPipeline> s_CubePipeline = nullptr;
    // std::shared_ptr<Camera> Renderer::s_CubesCamera = nullptr;

    // Line Data
    uint32_t s_LineCount = 0;
    std::vector<LineVertex> s_LineVertices(s_MaxLineVertexCount);
    std::shared_ptr<Buffer> s_LineVBO = nullptr;
    std::function<void()> s_LineBindFunc = nullptr;
    std::shared_ptr<GraphicsPipeline> s_LinePipeline = nullptr;

#if MESH
    // Mesh Data
    static std::vector<std::shared_ptr<Mesh>> Renderer::s_Meshes;
#endif

    // Texture Data
    uint32_t s_TextureUnitIndex = 0;
    std::array<std::shared_ptr<Texture>, s_MaxTextureUnits> s_TextureSlots;

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

        // Create renderer API object
        switch (s_RendererAPIType)
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't initialize renderer since window specified no renderer api");    
                return;
            case RendererAPIType::OpenGL:
                s_RendererAPI = std::make_unique<OpenGLRenderer>();
                PXL_ASSERT_MSG(s_RendererAPI, "Failed to create OpenGL renderer api object");

                break;
            case RendererAPIType::Vulkan:
                auto vulkanContext = static_pointer_cast<VulkanGraphicsContext>(window->GetGraphicsContext());
                PXL_ASSERT_MSG(vulkanContext, "Failed to retrieve VulkanGraphicsContext");

                s_RendererAPI = std::make_unique<VulkanRenderer>(vulkanContext);
                PXL_ASSERT_MSG(s_RendererAPI, "Failed to create Vulkan renderer api object");
                    
                break;
        }

        // --------------------
        // Prepare Quad Data
        // --------------------
        {
            // Prepare Quad Indices
            {
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
            
            const auto bufferLayout = QuadVertex::GetLayout();

            // Prepare Buffers
            s_QuadVBO = Buffer::Create(BufferUsage::Vertex, s_MaxQuadVertexCount * sizeof(QuadVertex));
            s_QuadIBO = Buffer::Create(BufferUsage::Index, s_MaxQuadIndexCount * sizeof(uint32_t), s_QuadIndices.data());
            s_StaticQuadVBO = Buffer::Create(BufferUsage::Vertex, s_MaxQuadVertexCount * sizeof(QuadVertex));

            // Shader storage
            std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.VertexLayout = bufferLayout;

            // Prepare other data based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_QuadVAO = VertexArray::Create();
                s_QuadVAO->AddVertexBuffer(s_QuadVBO, bufferLayout);
                s_QuadVAO->SetIndexBuffer(s_QuadIBO);

                s_StaticQuadVAO = VertexArray::Create();
                s_StaticQuadVAO->AddVertexBuffer(s_StaticQuadVBO, bufferLayout);
                s_StaticQuadVAO->SetIndexBuffer(s_QuadIBO); // Use the quad index buffer since it's also static

                s_QuadBufferBindFunc = []() {
                    s_QuadVAO->Bind();
                };

                s_StaticQuadBindFunc = []() {
                    s_StaticQuadVAO->Bind();
                };

                s_SetViewProjectionFunc = [&](const std::shared_ptr<GraphicsPipeline>& pipeline, const glm::mat4& vp) {
                    pipeline->SetUniformData("u_VP", BufferDataType::Mat4, &vp);
                };

                auto vertSrc = FileLoader::LoadGLSL("resources/shaders/quad_ogl.vert");
                auto fragSrc = FileLoader::LoadGLSL("resources/shaders/quad_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_QuadBufferBindFunc = []() {
                    s_QuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                s_StaticQuadBindFunc = []() {
                    s_StaticQuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                s_SetViewProjectionFunc = [&](const std::shared_ptr<GraphicsPipeline>& pipeline, const glm::mat4& vp) {
                    pipeline->SetPushConstantData("u_VP", &vp);
                };

                auto vertBin = FileLoader::LoadSPIRV("resources/shaders/compiled/quad_vert.spv");
                auto fragBin = FileLoader::LoadSPIRV("resources/shaders/compiled/quad_frag.spv");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertBin);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragBin);

                //s_QuadUniformLayout.Add({ "u_VertexColour", pxl::BufferDataType::Float4, pxl::ShaderStage::Vertex });

                pxl::PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", pxl::BufferDataType::Mat4, pxl::ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            pipelineSpecs.UniformLayout = s_QuadUniformLayout;

            s_QuadPipeline = GraphicsPipeline::Create(pipelineSpecs, shaders);
        }
        
        // --------------------
        // Prepare Cube Data
        // --------------------
        {
            // Prepare Cube Indices
            {
                uint32_t offset = 0;
                for (size_t i = 0; i < s_MaxCubeIndexCount; i += 6) // All this assumes the cube is made of 6 quads
                {
                    s_CubeIndices[i + 0] = 0 + offset;
                    s_CubeIndices[i + 1] = 1 + offset;
                    s_CubeIndices[i + 2] = 2 + offset;

                    s_CubeIndices[i + 3] = 2 + offset;
                    s_CubeIndices[i + 4] = 3 + offset;
                    s_CubeIndices[i + 5] = 0 + offset;

                    offset += 4;
                }
            }

            const auto bufferLayout = CubeVertex::GetLayout();

            // Prepare Buffers
            s_CubeVBO = Buffer::Create(BufferUsage::Vertex, s_MaxCubeVertexCount * sizeof(CubeVertex));
            s_CubeIBO = Buffer::Create(BufferUsage::Index, s_MaxCubeIndexCount * sizeof(uint32_t), s_CubeIndices.data());

            // Shader storage    
            std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.VertexLayout = bufferLayout;

            // Prepare other data based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_CubeVAO = VertexArray::Create();
                s_CubeVAO->AddVertexBuffer(s_CubeVBO, bufferLayout);
                s_CubeVAO->SetIndexBuffer(s_CubeIBO);

                s_CubeBindFunc = []() {
                    s_CubeVAO->Bind();
                };

                auto vertSrc = FileLoader::LoadGLSL("resources/shaders/quad_ogl.vert");
                auto fragSrc = FileLoader::LoadGLSL("resources/shaders/quad_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_CubeBindFunc = []() {
                    s_CubeVBO->Bind();
                    s_CubeIBO->Bind();
                };

                auto vertBin = FileLoader::LoadSPIRV("resources/shaders/compiled/quad_vert.spv");
                auto fragBin = FileLoader::LoadSPIRV("resources/shaders/compiled/quad_frag.spv");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertBin);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragBin);

                pxl::PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", pxl::BufferDataType::Mat4, pxl::ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_CubePipeline = GraphicsPipeline::Create(pipelineSpecs, shaders);
        }

        // --------------------
        // Prepare Line Data
        // --------------------
        {
            const auto bufferLayout = LineVertex::GetLayout();

            // Prepare Buffers
            s_LineVBO = Buffer::Create(BufferUsage::Vertex, s_MaxLineVertexCount * sizeof(LineVertex));

            // Shader storage
            std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Line;
            pipelineSpecs.VertexLayout = bufferLayout;

            // Prepare other data based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_LineVAO = VertexArray::Create();
                s_LineVAO->AddVertexBuffer(s_LineVBO, bufferLayout);

                s_LineBindFunc = []() {
                    s_LineVAO->Bind();
                };

                auto vertSrc = FileLoader::LoadGLSL("resources/shaders/line_ogl.vert");
                auto fragSrc = FileLoader::LoadGLSL("resources/shaders/line_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_LineBindFunc = []() {
                    s_LineVBO->Bind();
                };

                auto vertBin = FileLoader::LoadSPIRV("resources/shaders/compiled/line_vert.spv");
                auto fragBin = FileLoader::LoadSPIRV("resources/shaders/compiled/line_frag.spv");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertBin);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragBin);

                pxl::PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", pxl::BufferDataType::Mat4, pxl::ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_LinePipeline = GraphicsPipeline::Create(pipelineSpecs, shaders);
        }

    #if MESH
        {
            // Prepare Mesh VAO, VBO, IBO
            s_MeshVAO = std::make_shared<OpenGLVertexArray>(); // not api agnostic
            auto meshVBO = std::make_shared<OpenGLVertexBuffer>((uint32_t)(75 * sizeof(TriangleVertex)));
            auto meshIBO = std::make_shared<OpenGLIndexBuffer>(426);

            BufferLayout layout;
            layout.Add(3, BufferDataType::Float, false); // vertex position
            layout.Add(2, BufferDataType::Float, false); // texture coords

            s_MeshVAO->SetLayout(layout);
            s_MeshVAO->SetVertexBuffer(meshVBO);
            s_MeshVAO->SetIndexBuffer(meshIBO);
        }
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

#if CUSTOMRENDERING
    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& camera)
    {
        shader->Bind();
        shader->SetUniformMat4("u_VP", camera->GetViewProjectionMatrix());
    }

    void Renderer::Submit(const std::shared_ptr<GraphicsPipeline>& pipeline)
    {
        //auto vp = s_QuadsCamera->GetViewProjectionMatrix();
        pipeline->Bind();
        // TODO: set uniform variables
    }
#endif

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return;

        s_RendererAPI->Begin();
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
        
        constexpr glm::vec4 colour(1.0f);
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

        const auto layout = QuadVertex::GetLayout();

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

        if (s_QuadCount >= s_MaxQuadCount)
            Flush();

        const auto vertexCount = s_QuadCount * 4;
        constexpr float texIndex = -1.0f;
        
        s_QuadVertices[vertexCount + 0] = {{ position.x, position.y, position.z }, colour, { 0.0f, 0.0f }, texIndex };
        s_QuadVertices[vertexCount + 1] = {{ position.x + scale.x, position.y, position.z }, colour, { 1.0f, 0.0f }, texIndex };
        s_QuadVertices[vertexCount + 2] = {{ position.x + scale.x, position.y + scale.y, position.z }, colour, { 1.0f, 1.0f }, texIndex };
        s_QuadVertices[vertexCount + 3] = {{ position.x, position.y + scale.y, position.z }, colour, { 0.0f, 1.0f }, texIndex };

        s_QuadCount++;

    #ifdef PXL_DEBUG
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    #endif
    }

    void Renderer::AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::shared_ptr<Texture>& texture)
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

        constexpr glm::vec4 vertexColor(1.0f);
        const auto vertexCount = s_QuadCount * 4;
        
        s_QuadVertices[vertexCount + 0] = {{ position.x, position.y + scale.y, position.z }, vertexColor, { 0.0f, 1.0f }, textureIndex };
        s_QuadVertices[vertexCount + 1] = {{ position.x, position.y, position.z }, vertexColor, { 0.0f, 0.0f }, textureIndex };
        s_QuadVertices[vertexCount + 2] = {{ position.x + scale.x, position.y, position.z }, vertexColor, { 1.0f, 0.0f }, textureIndex };
        s_QuadVertices[vertexCount + 3] = {{ position.x + scale.x, position.y + scale.y, position.z }, vertexColor, { 1.0f, 1.0f }, textureIndex };

        s_QuadCount++;

    #ifdef PXL_DEBUG
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    #endif
    }

    void Renderer::AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;
        
        if (s_CubeCount >= s_MaxCubeCount)
            Flush();

        const auto vertexCount = s_CubeCount * 24;

        // Front
        s_CubeVertices[vertexCount + 0] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[vertexCount + 1] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[vertexCount + 2] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[vertexCount + 3] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Back
        s_CubeVertices[vertexCount + 4] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[vertexCount + 5] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[vertexCount + 6] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};
        s_CubeVertices[vertexCount + 7] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};

        // Left
        s_CubeVertices[vertexCount + 8]  = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[vertexCount + 9]  = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[vertexCount + 10] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[vertexCount + 11] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Top
        s_CubeVertices[vertexCount + 12] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[vertexCount + 13] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[vertexCount + 14] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[vertexCount + 15] = {{ position.x - 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Right
        s_CubeVertices[vertexCount + 16] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[vertexCount + 17] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[vertexCount + 18] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[vertexCount + 19] = {{ position.x + 0.5f * scale.x, position.y + 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        // Bottom
        s_CubeVertices[vertexCount + 20] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 0.0f, 0.0f }};
        s_CubeVertices[vertexCount + 21] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z - 0.5f * scale.z }, colour, { 1.0f, 0.0f }};
        s_CubeVertices[vertexCount + 22] = {{ position.x + 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 1.0f, 1.0f }};
        s_CubeVertices[vertexCount + 23] = {{ position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, position.z + 0.5f * scale.z }, colour, { 0.0f, 1.0f }};

        s_CubeCount++;

    #ifdef PXL_DEBUG
        s_Stats.CubeVertexCount += 4;
        s_Stats.CubeIndexCount += 6;
    #endif
    }

#ifdef TEXTUREDCUBE
    void Renderer::AddTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex)
    {
        constexpr uint32_t x = 2, y = 15;
        constexpr float sheetSize = 256.0f;
        constexpr float textureSize = 16.0f;
        glm::vec2 texCoords[] = {
            { (textureIndex * textureSize) / sheetSize, (y * textureSize) / sheetSize },
            { ((textureIndex + 1) * textureSize) / sheetSize, (y * textureSize) / sheetSize },
            { ((textureIndex + 1) * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
            { (textureIndex * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
        };
        
        for (int i = 0; i < 24; i += 4)
        {
            s_CubeMesh.Vertices[0 + i].TexCoords = texCoords[0]; 
            s_CubeMesh.Vertices[1 + i].TexCoords = texCoords[1]; 
            s_CubeMesh.Vertices[2 + i].TexCoords = texCoords[2]; 
            s_CubeMesh.Vertices[3 + i].TexCoords = texCoords[3]; 
        }
        s_CubeMesh.Translate(position.x, position.y, position.z);
        s_Meshes.push_back(s_CubeMesh);
    }
#endif

    void Renderer::AddLine(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;
        
        if (s_LineCount >= s_MaxLineCount)
            Flush();

        const auto vertexCount = s_LineCount * 2;
        
        s_LineVertices[vertexCount + 0] = {{ position1.x * scale.x, position1.y * scale.y, position1.z * scale.z }, colour };
        s_LineVertices[vertexCount + 1] = {{ position2.x * scale.x, position2.y * scale.y, position2.z * scale.z }, colour };

        s_LineCount++;

    #ifdef PXL_DEBUG
        s_Stats.LineVertexCount += 2;
    #endif
    }

#if MESH
    void Renderer::AddMesh(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        Submit(s_MeshVAO);
        s_RendererAPI->GetVertexArray()->GetVertexBuffer()->SetData(mesh->Vertices.size() * sizeof(TriangleVertex), mesh->Vertices.data());
        s_RendererAPI->GetVertexArray()->GetIndexBuffer()->SetData(mesh->Indices.size(), mesh->Indices.data());

        glm::mat4 transform = glm::translate(position) 
        * glm::rotate(glm::radians(rotation.y), glm::vec3(0, 1, 0)) 
        * glm::rotate(glm::radians(rotation.z), glm::vec3(0, 0, 1))
        * glm::rotate(glm::radians(rotation.x), glm::vec3(1, 0, 0)) // are these technically quaternion rotations?
        * glm::scale(scale);
        
        s_RendererAPI->GetShader()->SetUniformMat4("u_Transform", transform);

        DrawIndexed();
    }
#endif

    void Renderer::Flush()
    {
        PXL_PROFILE_SCOPE;
    
    #if TEXTURES
        // Bind textures
        for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
                s_TextureSlots[i]->Bind(i);
        
        s_TextureUnitIndex = 0;
    #endif

        // Flush quads if necessary
        if (s_QuadCount > 0)
        {
            s_QuadVBO->SetData(s_QuadCount * 4 * sizeof(QuadVertex), s_QuadVertices.data()); // THIS TAKES SIZE IN BYTES

            s_QuadBufferBindFunc();

        #if TEXTURES
            int32_t samplers[s_MaxTextureUnits];
            for (int32_t i = 0; i < s_MaxTextureUnits; i++)
                samplers[i] = i;

            s_QuadShader->SetUniformIntArray("u_Textures", samplers, s_MaxTextureUnits);
        #endif

            s_QuadPipeline->Bind();

            //auto vp = s_QuadsCamera->GetViewProjectionMatrix();
            s_SetViewProjectionFunc(s_QuadPipeline, s_QuadsCamera->GetViewProjectionMatrix());

            //glm::vec4 testVertexColour = { 1.0f, 0.7f, 0.2f, 1.0f };

            // TEMP
            //s_UniformBuffers[static_pointer_cast<VulkanGraphicsContext>(s_ContextHandle)->GetSwapchain()->GetCurrentImageIndex()]->SetData(s_QuadUniformLayout.GetSize(), &testVertexColour);

            s_RendererAPI->DrawIndexed(s_QuadCount * 6);
            s_QuadCount = 0;

        #ifdef PXL_DEBUG
            s_Stats.DrawCalls++;
        #endif
        }

        if (s_StaticQuadCount > 0)
        {
            // TODO: Implement
        }

        if (s_CubeCount > 0)
        {
            s_CubeVBO->SetData(s_CubeCount * 24 * sizeof(CubeVertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES
            
            s_CubeBindFunc();

            s_CubePipeline->Bind();
            
            s_SetViewProjectionFunc(s_CubePipeline, s_QuadsCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_CubeCount * 36);;
            s_CubeCount = 0;

        #ifdef PXL_DEBUG
            s_Stats.DrawCalls++;
        #endif
        }

        if (s_LineCount > 0)
        {
            s_LineVBO->SetData(s_LineCount * 2 * sizeof(LineVertex), s_LineVertices.data());
            
            s_LineBindFunc();

            s_LinePipeline->Bind();

            s_SetViewProjectionFunc(s_LinePipeline, s_QuadsCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawLines(s_LineCount * 2);
            s_LineCount = 0;
        
        #ifdef PXL_DEBUG
            s_Stats.DrawCalls++;
        #endif
        }
    }

    void Renderer::CalculateFPS()
    {
        float currentTime = (float)Platform::GetTime();
        float elapsedTime = currentTime - s_TimeAtLastFrame;

        if (elapsedTime > 0.05f)
        {
            s_Stats.FPS = s_FrameCount / elapsedTime;
            s_TimeAtLastFrame = currentTime;
            s_FrameCount = 0;
        }
    }
}