#include "Renderer.h"

#include "VertexArray.h"
#include "GPUBuffer.h"
#include "Core/Platform.h"
#include "OpenGL/OpenGLRenderer.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanContext.h"
#include "Utils/FileSystem.h"
#include "BufferLayout.h"
#include "UniformLayout.h"
#include "Debug/GUI/GUI.h"
#include "Vertices.h"

namespace pxl
{
    /* VERY IMPORTANT NOTE: Setting these max values to higher counts significantly increases compile times.
       TODO: Find some good values for these that are good for performance and don't kill the compiler */
    static constexpr uint32_t s_MaxQuadCount = 100;
    static constexpr uint32_t s_MaxQuadVertexCount = s_MaxQuadCount * 4;
    static constexpr uint32_t s_MaxQuadIndexCount = s_MaxQuadCount * 6;

    static constexpr uint32_t s_MaxCubeCount = 100;
    static constexpr uint32_t s_MaxCubeVertexCount = s_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved
    static constexpr uint32_t s_MaxCubeIndexCount = s_MaxCubeCount * 36;

    static constexpr uint32_t s_MaxLineCount = 100;
    static constexpr uint32_t s_MaxLineVertexCount = s_MaxLineCount * 2;

    static constexpr uint32_t s_MaxTextureUnits = 32; // TODO: This should be determined by a RenderCapabilities thing

    // Static Quad Data
    static uint32_t s_StaticQuadCount = 0;
    static std::shared_ptr<GPUBuffer> s_StaticQuadVBO = nullptr;
    static std::function<void()> s_StaticQuadBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO/IBO for Vulkan
    static std::array<QuadVertex, s_MaxQuadVertexCount> s_StaticQuadVertices;

    // Dynamic Quad Data
    static uint32_t s_QuadCount = 0;
    static std::array<QuadVertex, s_MaxQuadVertexCount> s_QuadVertices;
    static std::array<uint32_t, s_MaxQuadIndexCount> s_QuadIndices; // NOTE: currently also used by static quads
    static std::shared_ptr<GPUBuffer> s_QuadVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_QuadIBO = nullptr;
    static std::function<void()> s_QuadBufferBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO/IBO for Vulkan
    static std::function<void()> s_QuadUniformFunc = nullptr;
    static std::shared_ptr<GraphicsPipeline> s_QuadPipeline = nullptr;

    // Static Cube Data
    static uint32_t s_StaticCubeCount = 0;
    static std::shared_ptr<GPUBuffer> s_StaticCubeVBO = nullptr;
    static std::array<CubeVertex, s_MaxCubeVertexCount> s_StaticCubeVertices;

    // Dynamic Cube Data
    static uint32_t s_CubeCount = 0;
    static std::array<CubeVertex, s_MaxCubeVertexCount> s_CubeVertices;
    static std::array<uint32_t, s_MaxCubeIndexCount> s_CubeIndices;
    static std::shared_ptr<GPUBuffer> s_CubeVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_CubeIBO = nullptr;
    static std::function<void()> s_CubeBindFunc = nullptr;
    static std::shared_ptr<GraphicsPipeline> s_CubePipeline = nullptr;

    // Line Data
    static uint32_t s_LineCount = 0;
    static std::array<LineVertex, s_MaxLineVertexCount> s_LineVertices;
    static std::shared_ptr<GPUBuffer> s_LineVBO = nullptr;
    static std::function<void()> s_LineBindFunc = nullptr;
    static std::shared_ptr<GraphicsPipeline> s_LinePipeline = nullptr;

    // Mesh Data
    static std::array<MeshVertex, 1000> s_MeshVertices;
    static std::shared_ptr<GPUBuffer> s_MeshVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_MeshIBO = nullptr;
    static std::function<void()> s_MeshBindFunc = nullptr;
    static std::shared_ptr<GraphicsPipeline> s_MeshPipeline = nullptr;

    // Texture Data
    static uint32_t s_TextureUnitIndex = 0;
    static std::array<std::shared_ptr<Texture>, s_MaxTextureUnits> s_TextureUnits;
    static std::array<int32_t, s_MaxTextureUnits> s_Samplers;
    static std::shared_ptr<Texture> s_WhitePixelTexture = nullptr;

    // For OpenGL
    static std::shared_ptr<VertexArray> s_QuadVAO = nullptr;
    static std::shared_ptr<VertexArray> s_CubeVAO = nullptr;
    static std::shared_ptr<VertexArray> s_LineVAO = nullptr;
    static std::shared_ptr<VertexArray> s_MeshVAO = nullptr;
    static std::shared_ptr<VertexArray> s_StaticQuadVAO = nullptr; // TODO: remove this? it feels unnecessary when the buffer layout is the same.
    
    static std::function<void(const std::shared_ptr<GraphicsPipeline>&, const glm::mat4& vp)> s_SetViewProjectionFunc = nullptr;

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
                constexpr std::array<uint32_t, 6> defaultIndices = Quad::GetDefaultIndices();

                uint32_t offset = 0;
                for (size_t i = 0; i < s_MaxQuadIndexCount; i += 6)
                {
                    for (uint32_t j = 0; j < 6; j++)
                    {
                        s_QuadIndices[i + j] = defaultIndices[j] + offset;
                    }

                    offset += 4;
                }
            }
            
            const auto bufferLayout = QuadVertex::GetLayout();

            // Prepare Buffers
            s_QuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, s_MaxQuadVertexCount * sizeof(QuadVertex));
            s_QuadIBO = GPUBuffer::Create(GPUBufferUsage::Index, s_MaxQuadIndexCount * sizeof(uint32_t), s_QuadIndices.data());
            s_StaticQuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, s_MaxQuadVertexCount * sizeof(QuadVertex));

            // Shader storage
            std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.PolygonFillMode = PolygonFillMode::Fill;
            pipelineSpecs.CullMode = CullMode::None;
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

                s_QuadBufferBindFunc = [&]() {
                    s_QuadVAO->Bind();
                };

                s_StaticQuadBindFunc = [&]() {
                    s_StaticQuadVAO->Bind();
                };

                s_SetViewProjectionFunc = [&](const std::shared_ptr<GraphicsPipeline>& pipeline, const glm::mat4& vp) {
                    pipeline->SetUniformData("u_VP", UniformDataType::Mat4, &vp);
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_textured_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_textured_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_QuadBufferBindFunc = [&]() {
                    s_QuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                s_StaticQuadBindFunc = [&]() {
                    s_StaticQuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                s_SetViewProjectionFunc = [&](const std::shared_ptr<GraphicsPipeline>& pipeline, const glm::mat4& vp) {
                    pipeline->SetPushConstantData("u_VP", &vp);
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_vk.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_vk.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

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

            // Assign default cube positions
            {
                // Front
                s_DefaultCubePositions[0] = { -0.5, -0.5f, 0.5f, 1.0f };
                s_DefaultCubePositions[1] = {  0.5, -0.5f, 0.5f, 1.0f };
                s_DefaultCubePositions[2] = {  0.5,  0.5f, 0.5f, 1.0f };
                s_DefaultCubePositions[3] = { -0.5,  0.5f, 0.5f, 1.0f };

                // Back
                s_DefaultCubePositions[4] = { -0.5, -0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[5] = {  0.5, -0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[6] = {  0.5,  0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[7] = { -0.5,  0.5f, -0.5f, 1.0f };

                // Left
                s_DefaultCubePositions[8]  = { -0.5, -0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[9]  = { -0.5, -0.5f,  0.5f, 1.0f };
                s_DefaultCubePositions[10] = { -0.5,  0.5f,  0.5f, 1.0f };
                s_DefaultCubePositions[11] = { -0.5,  0.5f, -0.5f, 1.0f };

                // Top
                s_DefaultCubePositions[12] = { -0.5, 0.5f,  0.5f, 1.0f };
                s_DefaultCubePositions[13] = {  0.5, 0.5f,  0.5f, 1.0f };
                s_DefaultCubePositions[14] = {  0.5, 0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[15] = { -0.5, 0.5f, -0.5f, 1.0f };

                // Right
                s_DefaultCubePositions[16] = { 0.5, -0.5f,  0.5f, 1.0f };
                s_DefaultCubePositions[17] = { 0.5, -0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[18] = { 0.5,  0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[19] = { 0.5,  0.5f,  0.5f, 1.0f };

                // Bottom
                s_DefaultCubePositions[20] = { -0.5, -0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[21] = {  0.5, -0.5f, -0.5f, 1.0f };
                s_DefaultCubePositions[22] = {  0.5, -0.5f,  0.5f, 1.0f };
                s_DefaultCubePositions[23] = { -0.5, -0.5f,  0.5f, 1.0f };
            }

            const auto bufferLayout = CubeVertex::GetLayout();

            // Prepare Buffers
            s_CubeVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, s_MaxCubeVertexCount * sizeof(CubeVertex));
            s_CubeIBO = GPUBuffer::Create(GPUBufferUsage::Index, s_MaxCubeIndexCount * sizeof(uint32_t), s_CubeIndices.data());

            // Shader storage    
            std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.CullMode = CullMode::Back;
            pipelineSpecs.VertexLayout = bufferLayout;

            // Prepare other data based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_CubeVAO = VertexArray::Create();
                s_CubeVAO->AddVertexBuffer(s_CubeVBO, bufferLayout);
                s_CubeVAO->SetIndexBuffer(s_CubeIBO);

                s_CubeBindFunc = [&]() {
                    s_CubeVAO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_CubeBindFunc = [&]() {
                    s_CubeVBO->Bind();
                    s_CubeIBO->Bind();
                };

                auto vertBin = FileSystem::LoadSPIRV("resources/shaders/compiled/quad_vert.spv");
                auto fragBin = FileSystem::LoadSPIRV("resources/shaders/compiled/quad_frag.spv");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertBin);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragBin);

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

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
            s_LineVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, s_MaxLineVertexCount * sizeof(LineVertex));

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

                s_LineBindFunc = [&]() {
                    s_LineVAO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/line_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/line_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_LineBindFunc = [&]() {
                    s_LineVBO->Bind();
                };

                auto vertBin = FileSystem::LoadSPIRV("resources/shaders/compiled/line_vert.spv");
                auto fragBin = FileSystem::LoadSPIRV("resources/shaders/compiled/line_frag.spv");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertBin);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragBin);

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_LinePipeline = GraphicsPipeline::Create(pipelineSpecs, shaders);
        }

        // --------------------
        // Prepare Mesh Data
        // --------------------
        {
            const auto bufferLayout = MeshVertex::GetLayout();

            // Prepare Buffers
            s_MeshVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, 1000 * sizeof(MeshVertex));
            s_MeshIBO = GPUBuffer::Create(GPUBufferUsage::Index, 1000 * sizeof(uint32_t));

            // Shader storage
            std::unordered_map<ShaderStage, std::shared_ptr<Shader>> shaders;

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.VertexLayout = bufferLayout;

            // Prepare other data based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_MeshVAO = VertexArray::Create();
                s_MeshVAO->AddVertexBuffer(s_MeshVBO, bufferLayout);
                s_MeshVAO->SetIndexBuffer(s_MeshIBO);

                s_MeshBindFunc = [&]() {
                    s_MeshVAO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_MeshBindFunc = [&]() {
                    s_MeshVBO->Bind();
                    s_MeshIBO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_vk.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_vk.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_MeshPipeline = GraphicsPipeline::Create(pipelineSpecs, shaders);
        }

        // Prepare texture data
        // TODO: hard code this
        s_WhitePixelTexture = FileSystem::LoadTextureFromImage("resources/textures/whiteTexture.bmp");
        
        // Set samplers
        for (int32_t i = 0; i < s_MaxTextureUnits; i++)
            s_Samplers[i] = i;

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

        PXL_LOG_INFO(LogArea::Renderer, "Renderer shutdown");
    }

    void Renderer::Clear()
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT_MSG(s_Enabled, "Renderer isn't initialized")
        
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
        
        // NOTE: required atm otherwise application will call it even if the renderer isn't enabled
        if (!s_Enabled)
            return;

        s_RendererAPI->Begin();

        s_TextureUnits[0] = s_WhitePixelTexture;
        s_TextureUnitIndex++;
    }

    void Renderer::End()
    {
        PXL_PROFILE_SCOPE;

        // NOTE: required atm otherwise application will call it even if the renderer isn't enabled
        if (!s_Enabled)
            return;
        
        Flush();

        if (GUI::IsInitialized())
        {
            GUI::Update();
            GUI::Render();
        }

        s_RendererAPI->End();

        ResetStats();
    }

    void Renderer::AddStaticQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;
        
        const auto vertexCount = s_StaticQuadCount * 4;

        glm::mat4 transform = CalculateTransform(position, rotation, { scale.x, scale.y, 1.0f });
        
        std::array<QuadVertex, 4> defaultVertices = Quad::GetDefaultVertices();

        for (uint32_t i = 0; i < 4; i++)
        {
            s_StaticQuadVertices[vertexCount + i] = { 
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = colour, 
                .TexCoords = defaultVertices[i].TexCoords,
                .TexIndex = -1.0f,
            };
        }

        s_StaticQuadCount++;
    }

    void Renderer::AddStaticCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;

        const auto vertexCount = s_StaticCubeCount * 24;

        glm::mat4 transform = CalculateTransform(position, rotation, scale);

        for (uint32_t i = 0; i < 4; i++)
            s_StaticCubeVertices[vertexCount + i] = { transform * s_DefaultCubePositions[i], colour, glm::vec2(0.0f) };

        s_StaticCubeCount++;

    #ifdef PXL_DEBUG
        s_Stats.CubeVertexCount += 4;
        s_Stats.CubeIndexCount += 6;
    #endif
    }

    void Renderer::StaticGeometryReady()
    {
        PXL_PROFILE_SCOPE;
        
        if (s_StaticQuadCount > 0)
            s_StaticQuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, s_StaticQuadCount * 4 * sizeof(QuadVertex), s_StaticQuadVertices.data());
        
        if (s_StaticCubeCount > 0)
            s_StaticCubeVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, s_StaticCubeCount * 24 * sizeof(CubeVertex), s_StaticCubeVertices.data());

        const auto layout = QuadVertex::GetLayout();

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            s_StaticQuadVAO->AddVertexBuffer(s_StaticQuadVBO, layout);
            s_StaticQuadVAO->SetIndexBuffer(s_QuadIBO);
        }
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;

        if (s_QuadCount >= s_MaxQuadCount)
            Flush();

        const auto vertexCount = s_QuadCount * 4;

        glm::mat4 transform = CalculateTransform(position, rotation, { scale.x, scale.y, 1.0f });

        constexpr std::array<QuadVertex, 4> defaultVertices = Quad::GetDefaultVertices();

        for (uint32_t i = 0; i < 4; i++)
        {
            s_QuadVertices[vertexCount + i] = {
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = colour, 
                .TexCoords = defaultVertices[i].TexCoords,
                .TexIndex = 0.0f,
            };
        }

        s_QuadCount++;

    #ifdef PXL_DEBUG
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    #endif
    }

    void Renderer::AddQuad(const Quad& quad)
    {
        AddQuad(quad.GetPositionWithOrigin(), quad.Rotation, quad.Size, quad.Colour);
    }

    void Renderer::AddTexturedQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const std::shared_ptr<Texture>& texture)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT_MSG(texture, "Texture is invalid");
        
        if (s_QuadCount >= s_MaxQuadCount)
            Flush();

        if (s_TextureUnitIndex >= s_MaxTextureUnits)
            Flush();

        float textureIndex = -1.0f;

        bool foundTexture = false;
        for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
        {
            if (texture == s_TextureUnits[i])
            {
                textureIndex = static_cast<float>(i);
                foundTexture = true;
                break;
            }
        }

        if (!foundTexture)
        {
            textureIndex = static_cast<float>(s_TextureUnitIndex);
			s_TextureUnits[s_TextureUnitIndex] = texture;
			s_TextureUnitIndex++;
        }

        constexpr glm::vec4 vertexColor(1.0f);
        const auto vertexCount = s_QuadCount * 4;

        glm::mat4 transform = CalculateTransform(position, rotation, { scale.x, scale.y, 1.0f });

        constexpr std::array<QuadVertex, 4> defaultVertices = Quad::GetDefaultVertices();

        for (uint32_t i = 0; i < 4; i++)
        {
            s_QuadVertices[vertexCount + i] = { 
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = vertexColor, 
                .TexCoords = defaultVertices[i].TexCoords,
                .TexIndex = textureIndex,
            };
        }

        s_QuadCount++;

    #ifdef PXL_DEBUG
        s_Stats.QuadVertexCount += 4;
        s_Stats.QuadIndexCount += 6;
    #endif
    }

    void Renderer::AddTexturedQuad(const Quad& quad, const std::shared_ptr<Texture>& texture)
    {  
        AddTexturedQuad(quad.GetPositionWithOrigin(), quad.Rotation, quad.Size, texture);
    }

    void Renderer::AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        PXL_PROFILE_SCOPE;
        
        if (s_CubeCount >= s_MaxCubeCount)
            Flush();

        const auto vertexCount = s_CubeCount * 24;

        glm::mat4 transform = CalculateTransform(position, rotation, scale);

        for (uint32_t i = 0; i < 24; i++)
            s_CubeVertices[vertexCount + i] = { transform * s_DefaultCubePositions[i], colour, { 0.0f, 0.0f }}; // NOTE: TexCoords are incorrect here

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

    void Renderer::AddLine(const glm::vec3& position1, const glm::vec3& position2, [[maybe_unused]] const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
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

    void Renderer::DrawMesh(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        PXL_PROFILE_SCOPE;
        
        if (!mesh)
        {
            PXL_LOG_WARN(LogArea::Renderer, "Failed to draw mesh as it was invalid");
            return;
        }

        glm::mat4 transform = CalculateTransform(position, rotation, scale);

        uint32_t vertexOffset = 0;
        for (auto& vertex : mesh->Vertices)
        {
            s_MeshVertices[vertexOffset] = { transform * glm::vec4(vertex.Position, 1.0f), vertex.Colour, vertex.TexCoords, vertex.TexIndex };
            vertexOffset++;
        }
        
        s_MeshPipeline->Bind();

        s_MeshVBO->SetData(static_cast<uint32_t>(mesh->Vertices.size() * sizeof(MeshVertex)), s_MeshVertices.data());
        s_MeshIBO->SetData(static_cast<uint32_t>(mesh->Indices.size() * sizeof(uint32_t)), mesh->Indices.data());

        s_MeshBindFunc();

        s_SetViewProjectionFunc(s_MeshPipeline, s_QuadCamera->GetViewProjectionMatrix());

        s_RendererAPI->DrawIndexed(static_cast<uint32_t>(mesh->Indices.size()));

    #ifdef PXL_DEBUG
        s_Stats.DrawCalls++;
        s_Stats.MeshCount++;
        s_Stats.MeshVertexCount += static_cast<uint32_t>(s_MeshVertices.size());
        s_Stats.MeshIndexCount += static_cast<uint32_t>(mesh->Indices.size());
    #endif
    }

    void Renderer::Flush()
    {
        PXL_PROFILE_SCOPE;

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            // Bind textures
            for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
            {
                s_TextureUnits[i]->Bind(i);
                s_Stats.TextureBinds++;
            }

            s_TextureUnitIndex = 0;
        }

        // Flush quads if necessary
        if (s_QuadCount > 0)
        {
            PXL_ASSERT_MSG(s_QuadCamera, "Quad Camera isn't set");
            
            s_QuadVBO->SetData(s_QuadCount * 4 * sizeof(QuadVertex), s_QuadVertices.data()); // THIS TAKES SIZE IN BYTES

            s_QuadBufferBindFunc();

            if (s_RendererAPIType == RendererAPIType::OpenGL)
                s_QuadPipeline->SetUniformData("u_Textures", UniformDataType::IntArray, s_MaxTextureUnits, s_Samplers.data());

            s_QuadPipeline->Bind();

            s_SetViewProjectionFunc(s_QuadPipeline, s_QuadCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_QuadCount * 6);
            s_QuadCount = 0;

        #ifdef PXL_DEBUG
            s_Stats.DrawCalls++;
        #endif
        }

        // Flush static quads if necessary
        if (s_StaticQuadCount > 0)
        {
            PXL_ASSERT_MSG(s_QuadCamera, "Quad camera isn't set");

            s_StaticQuadBindFunc();

            s_QuadPipeline->Bind();

            s_SetViewProjectionFunc(s_QuadPipeline, s_QuadCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_StaticQuadCount * 6); // TODO: use a cached s_StaticQuadIndexCount variable so it doesnt have to math everytime
        }

        // Flush cubes if necessary
        if (s_CubeCount > 0)
        {
            PXL_ASSERT_MSG(s_CubeCamera, "Cube camera isn't set");

            s_CubeVBO->SetData(s_CubeCount * 24 * sizeof(CubeVertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES
            
            s_CubeBindFunc();

            s_CubePipeline->Bind();
            
            s_SetViewProjectionFunc(s_CubePipeline, s_QuadsCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_CubeCount * 36);
            s_CubeCount = 0;

        #ifdef PXL_DEBUG
            s_Stats.DrawCalls++;
        #endif
        }
        
        // Flush static cubes if necessary
        if (s_StaticCubeCount > 0)
        {
            PXL_ASSERT_MSG(s_CubeCamera, "Cube camera isn't set");

            // TODO: Move this into a StaticCubeBind lambda
            s_StaticCubeVBO->Bind();
            s_CubeIBO->Bind();

            s_CubePipeline->Bind();

            s_SetViewProjectionFunc(s_CubePipeline, s_QuadsCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_StaticCubeCount * 36);
        }

        // Flush lines if necessary
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

    glm::mat4 Renderer::CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        return glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) 
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
            * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, scale.z });
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