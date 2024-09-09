#include "Renderer.h"

#include "BufferLayout.h"
#include "Core/Platform.h"
#include "Debug/GUI/GUI.h"
#include "GPUBuffer.h"
#include "OpenGL/OpenGLRenderer.h"
#include "UniformLayout.h"
#include "Utils/FileSystem.h"
#include "VertexArray.h"
#include "Vertices.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanRenderer.h"

namespace pxl
{
    /* VERY IMPORTANT NOTE: Setting these max values to higher counts significantly increases compile times.
       TODO: Find some good values for these that are good for performance and don't kill the compiler */
    static constexpr uint32_t k_MaxQuadCount = 100;
    static constexpr uint32_t k_MaxQuadVertexCount = k_MaxQuadCount * 4;
    static constexpr uint32_t k_MaxQuadIndexCount = k_MaxQuadCount * 6;

    static constexpr uint32_t k_MaxCubeCount = 100;
    static constexpr uint32_t k_MaxCubeVertexCount = k_MaxCubeCount * 24; // textures break on 8 vertex cubes, need to look into how this can be solved
    static constexpr uint32_t k_MaxCubeIndexCount = k_MaxCubeCount * 36;

    static constexpr uint32_t k_MaxLineCount = 100;
    static constexpr uint32_t k_MaxLineVertexCount = k_MaxLineCount * 2;

    static constexpr uint32_t k_MaxTextureUnits = 32; // TODO: This should be determined by a RenderCapabilities thing

    // Static Quad Data
    static std::shared_ptr<GPUBuffer> s_StaticQuadVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_StaticQuadIBO = nullptr;

    static std::function<void()> s_StaticQuadBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO/IBO for Vulkan

    static std::vector<QuadVertex> s_StaticQuadVertices;
    static std::vector<uint32_t> s_StaticQuadIndices;

    // Dynamic Quad Data
    static uint32_t s_QuadCount = 0;

    static std::array<QuadVertex, k_MaxQuadVertexCount> s_QuadVertices;
    static std::array<uint32_t, k_MaxQuadIndexCount> s_QuadIndices; // NOTE: currently also used by static quads

    static std::shared_ptr<GPUBuffer> s_QuadVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_QuadIBO = nullptr;

    static std::function<void()> s_QuadBufferBindFunc = nullptr; // NOTE: Lambda that binds VAO for OpenGL and VBO/IBO for Vulkan
    static std::function<void()> s_QuadUniformFunc = nullptr;

    static std::shared_ptr<GraphicsPipeline> s_QuadPipeline = nullptr;

    // Static Cube Data
    static uint32_t s_StaticQuadIndexOffset = 0;

    static std::shared_ptr<GPUBuffer> s_StaticCubeVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_StaticCubeIBO = nullptr;

    static std::vector<CubeVertex> s_StaticCubeVertices;
    static std::vector<uint32_t> s_StaticCubeIndices;

    // Dynamic Cube Data
    static uint32_t s_CubeCount = 0;

    static std::array<CubeVertex, k_MaxCubeVertexCount> s_CubeVertices;
    static std::array<uint32_t, k_MaxCubeIndexCount> s_CubeIndices;

    static std::shared_ptr<GPUBuffer> s_CubeVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_CubeIBO = nullptr;

    static std::function<void()> s_CubeBindFunc = nullptr;

    static std::shared_ptr<GraphicsPipeline> s_CubePipeline = nullptr;

    // Line Data
    static uint32_t s_LineCount = 0;

    static std::array<LineVertex, k_MaxLineVertexCount> s_LineVertices;

    static std::shared_ptr<GPUBuffer> s_LineVBO = nullptr;

    static std::function<void()> s_LineBindFunc = nullptr;

    static std::shared_ptr<GraphicsPipeline> s_LinePipeline = nullptr;

    // Mesh Data
    static std::vector<MeshVertex> s_MeshVertices;

    static std::shared_ptr<GPUBuffer> s_MeshVBO = nullptr;
    static std::shared_ptr<GPUBuffer> s_MeshIBO = nullptr;

    static std::function<void()> s_MeshBindFunc = nullptr;

    static std::shared_ptr<GraphicsPipeline> s_MeshPipeline = nullptr;

    // Texture Data
    static uint32_t s_TextureUnitIndex = 0;

    static std::array<std::shared_ptr<Texture>, k_MaxTextureUnits> s_TextureUnits;
    static std::array<int32_t, k_MaxTextureUnits> s_Samplers;

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
            case RendererAPIType::None: PXL_LOG_ERROR(LogArea::Renderer, "Can't initialize renderer since window specified no renderer api"); return;

            case RendererAPIType::OpenGL:
                s_RendererAPI = std::make_unique<OpenGLRenderer>();
                PXL_ASSERT_MSG(s_RendererAPI, "Failed to create OpenGL renderer api object");
                break;

            case RendererAPIType::Vulkan:
                auto vulkanContext = static_pointer_cast<VulkanGraphicsContext>(window->GetGraphicsContext());
                PXL_ASSERT_MSG(vulkanContext, "Failed to retrieve VulkanGraphicsContext");

                VulkanDeletionQueue::Init(static_pointer_cast<VulkanDevice>(window->GetGraphicsContext()->GetDevice()));

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
                for (size_t i = 0; i < k_MaxQuadIndexCount; i += 6)
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
            s_QuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, k_MaxQuadVertexCount * sizeof(QuadVertex));
            s_QuadIBO = GPUBuffer::Create(GPUBufferUsage::Index, k_MaxQuadIndexCount * sizeof(uint32_t), s_QuadIndices.data());
            s_StaticQuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, k_MaxQuadVertexCount * sizeof(QuadVertex));

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

                s_QuadBufferBindFunc = [&]()
                {
                    s_QuadVAO->Bind();
                };

                s_StaticQuadBindFunc = [&]()
                {
                    s_StaticQuadVAO->Bind();
                };

                s_SetViewProjectionFunc = [&](const std::shared_ptr<GraphicsPipeline>& pipeline, const glm::mat4& vp)
                {
                    pipeline->SetUniformData("u_VP", UniformDataType::Mat4, &vp);
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_textured_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_textured_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_QuadBufferBindFunc = [&]()
                {
                    s_QuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                s_StaticQuadBindFunc = [&]()
                {
                    s_StaticQuadVBO->Bind();
                    s_QuadIBO->Bind();
                };

                s_SetViewProjectionFunc = [&](const std::shared_ptr<GraphicsPipeline>& pipeline, const glm::mat4& vp)
                {
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
                constexpr std::array<uint32_t, 6> defaultIndices = Cube::GetDefaultIndices();

                uint32_t offset = 0;
                for (size_t i = 0; i < k_MaxCubeIndexCount; i += 6)
                {
                    for (uint32_t j = 0; j < 6; j++)
                    {
                        s_CubeIndices[i + j] = defaultIndices[j] + offset;
                    }

                    offset += 4;
                }
            }

            const auto bufferLayout = CubeVertex::GetLayout();

            // Prepare Buffers
            s_CubeVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, k_MaxCubeVertexCount * sizeof(CubeVertex));
            s_CubeIBO = GPUBuffer::Create(GPUBufferUsage::Index, k_MaxCubeIndexCount * sizeof(uint32_t), s_CubeIndices.data());

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

                s_CubeBindFunc = [&]()
                {
                    s_CubeVAO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_CubeBindFunc = [&]()
                {
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
            s_LineVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, k_MaxLineVertexCount * sizeof(LineVertex));

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

                s_LineBindFunc = [&]()
                {
                    s_LineVAO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/line_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/line_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_LineBindFunc = [&]()
                {
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

                s_MeshBindFunc = [&]()
                {
                    s_MeshVAO->Bind();
                };

                auto vertSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.vert");
                auto fragSrc = FileSystem::LoadGLSL("resources/shaders/quad_ogl.frag");

                shaders[ShaderStage::Vertex] = Shader::Create(ShaderStage::Vertex, vertSrc);
                shaders[ShaderStage::Fragment] = Shader::Create(ShaderStage::Fragment, fragSrc);
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_MeshBindFunc = [&]()
                {
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
        for (int32_t i = 0; i < k_MaxTextureUnits; i++)
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

    void Renderer::SetClearColour(const glm::vec4& colour)
    {
        s_RendererAPI->SetClearColour(colour);
    }

    void Renderer::SetClearColour(ColourName colour)
    {
        s_RendererAPI->SetClearColour(Colour::GetColourAsVec4(colour));
    }

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;

        // NOTE: required atm otherwise application will call it even if the renderer isn't enabled
        if (!s_Enabled)
            return;

        s_RendererAPI->Begin();

        // Clear the screen
        s_RendererAPI->Clear();

        // Set first texture unit as white pixel texture
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

    void Renderer::SetCamera(RendererGeometryTarget target, const std::shared_ptr<Camera>& camera)
    {
        PXL_ASSERT(camera);

        switch (target)
        {
            case RendererGeometryTarget::All:
                s_QuadCamera = camera;
                s_CubeCamera = camera;
                s_LineCamera = camera;
                return;
            case RendererGeometryTarget::Quad: s_QuadCamera = camera; return;
            case RendererGeometryTarget::Cube: s_CubeCamera = camera; return;
            case RendererGeometryTarget::Line: s_LineCamera = camera; return;
            case RendererGeometryTarget::Mesh: PXL_LOG_WARN(LogArea::Renderer, "Mesh cameras aren't supported yet"); return;
        }
    }

    void Renderer::SetPipeline(RendererGeometryTarget target, const std::shared_ptr<GraphicsPipeline>& pipeline)
    {
        PXL_ASSERT(pipeline);

        switch (target)
        {
            case RendererGeometryTarget::All:
                s_QuadPipeline = pipeline;
                s_CubePipeline = pipeline;
                s_LinePipeline = pipeline;
                return;
            case RendererGeometryTarget::Quad: s_QuadPipeline = pipeline; return;
            case RendererGeometryTarget::Cube: s_CubePipeline = pipeline; return;
            case RendererGeometryTarget::Line: s_LinePipeline = pipeline; return;
            case RendererGeometryTarget::Mesh: s_MeshPipeline = pipeline; return;
        }
    }

    void Renderer::AddQuad(const Quad& quad)
    {
        PXL_PROFILE_SCOPE;

        if (s_QuadCount >= k_MaxQuadCount)
            Flush();

        float texIndex = 0.0f;
        if (quad.Texture.has_value())
        {
            PXL_ASSERT(quad.Texture.value());
            texIndex = GetTextureIndex(quad.Texture.value());
        }

        std::array<QuadVertex, 4> defaultVertices = Quad::GetDefaultVertices();

        std::array<glm::vec2, 4> texCoords = Quad::GetDefaultTexCoords();
        if (quad.TextureUV.has_value())
            texCoords = quad.TextureUV.value();

        const auto vertexCount = s_QuadCount * 4;

        glm::mat4 transform = CalculateTransform(quad.Position, quad.Rotation, glm::vec3(quad.Size, 1.0f));

        for (uint32_t i = 0; i < 4; i++)
        {
            s_QuadVertices[vertexCount + i] = {
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = quad.Colour,
                .TexCoords = texCoords[i],
                .TexIndex = texIndex,
            };
        }

        s_QuadCount++;
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour)
    {
        AddQuad({ position, rotation, scale, colour });
    }

    void Renderer::AddCube(const Cube& cube)
    {
        PXL_PROFILE_SCOPE;

        if (s_CubeCount >= k_MaxCubeCount)
            Flush();

        const auto vertexCount = s_CubeCount * 24;

        glm::mat4 transform = CalculateTransform(cube.Position, cube.Rotation, cube.Size);

        constexpr std::array<CubeVertex, 24> defaultVertices = Cube::GetDefaultVertices();

        for (uint32_t i = 0; i < 24; i++)
        {
            s_CubeVertices[vertexCount + i] = {
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = cube.Colour,
                .TexCoords = { 0.0f, 0.0f }, // NOTE: TexCoords are incorrect here
                .TexIndex = 0.0f,
            };
        }

        s_CubeCount++;
    }

    void Renderer::AddCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        AddCube({ position, rotation, scale, colour });
    }

#ifdef TEXTUREDCUBE
    void Renderer::AddTexturedCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, uint32_t textureIndex)
    {
        constexpr uint32_t x = 2, y = 15;
        constexpr float sheetSize = 256.0f;
        constexpr float textureSize = 16.0f;
        glm::vec2 texCoords[] = {
            {       (textureIndex * textureSize) / sheetSize,       (y * textureSize) / sheetSize },
            { ((textureIndex + 1) * textureSize) / sheetSize,       (y * textureSize) / sheetSize },
            { ((textureIndex + 1) * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
            {       (textureIndex * textureSize) / sheetSize, ((y + 1) * textureSize) / sheetSize },
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

        if (s_LineCount >= k_MaxLineCount)
            Flush();

        const auto vertexCount = s_LineCount * 2;

        s_LineVertices[vertexCount + 0] = {
            .Position = { position1.x * scale.x, position1.y * scale.y, position1.z * scale.z },
            .Colour = colour,
        };
        s_LineVertices[vertexCount + 1] = {
            .Position = { position2.x * scale.x, position2.y * scale.y, position2.z * scale.z },
            .Colour = colour,
        };

        s_LineCount++;
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

        for (auto& vertex : mesh->Vertices)
        {
            s_MeshVertices.push_back({ transform * glm::vec4(vertex.Position, 1.0f), vertex.Colour, vertex.TexCoords, vertex.TexIndex });
        }

        s_MeshPipeline->Bind();

        s_MeshVBO->SetData(static_cast<uint32_t>(mesh->Vertices.size() * sizeof(MeshVertex)), s_MeshVertices.data());
        s_MeshIBO->SetData(static_cast<uint32_t>(mesh->Indices.size() * sizeof(uint32_t)), mesh->Indices.data());

        s_MeshBindFunc();

        s_SetViewProjectionFunc(s_MeshPipeline, s_QuadCamera->GetViewProjectionMatrix());

        s_RendererAPI->DrawIndexed(static_cast<uint32_t>(mesh->Indices.size()));

        s_Stats.PipelineBinds++;
        s_Stats.DrawCalls++;
        s_Stats.MeshCount++;
        s_Stats.MeshVertexCount += static_cast<uint32_t>(s_MeshVertices.size());
        s_Stats.MeshIndexCount += static_cast<uint32_t>(mesh->Indices.size());

        s_MeshVertices.clear();
    }

    void Renderer::ResetStaticGeometry(RendererGeometryTarget target)
    {
        switch (target)
        {
            case RendererGeometryTarget::All:
                s_StaticQuadVertices.clear();
                s_StaticCubeVertices.clear();
                return;

            case RendererGeometryTarget::Quad: s_StaticQuadVertices.clear(); return;
            case RendererGeometryTarget::Cube: s_StaticCubeVertices.clear(); return;
            case RendererGeometryTarget::Line: PXL_LOG_ERROR(LogArea::Renderer, "Static Lines aren't supported"); return;
            case RendererGeometryTarget::Mesh: PXL_LOG_ERROR(LogArea::Renderer, "Static Meshes aren't supported"); return;
        }
    }

    void Renderer::AddStaticQuad(const Quad& quad)
    {
        PXL_PROFILE_SCOPE;

        if (quad.Texture.has_value())
            PXL_LOG_WARN(LogArea::Renderer, "Static geometry doesn't support textures yet");

        constexpr std::array<QuadVertex, 4> defaultVertices = Quad::GetDefaultVertices();
        constexpr std::array<uint32_t, 6> defaultIndices = Quad::GetDefaultIndices();

        glm::mat4 transform = CalculateTransform(quad.Position, quad.Rotation, glm::vec3(quad.Size, 1.0f));

        // Add vertices
        for (uint32_t i = 0; i < 4; i++)
        {
            s_StaticQuadVertices.push_back({
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = quad.Colour,
                .TexCoords = defaultVertices[i].TexCoords,
                .TexIndex = 0.0f,
            });
        }

        // Add indices
        for (uint32_t j = 0; j < 6; j++)
            s_StaticQuadIndices.push_back(s_StaticQuadIndexOffset + defaultIndices[j]);

        s_StaticQuadIndexOffset += 4;
    }

    void Renderer::AddStaticQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale, const glm::vec4& colour)
    {
        AddStaticQuad({ position, rotation, scale, colour });
    }

    void Renderer::AddStaticCube(const Cube& cube)
    {
        PXL_PROFILE_SCOPE;

        glm::mat4 transform = CalculateTransform(cube.Position, cube.Rotation, cube.Size);

        constexpr std::array<CubeVertex, 24> defaultVertices = Cube::GetDefaultVertices();

        for (uint32_t i = 0; i < 24; i++)
        {
            s_StaticCubeVertices.push_back({
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = cube.Colour,
                .TexCoords = defaultVertices[i].TexCoords,
                .TexIndex = 0.0f,
            });
        }
    }

    void Renderer::AddStaticCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& colour)
    {
        AddStaticCube({ position, rotation, scale, colour });
    }

    void Renderer::StaticGeometryReady()
    {
        PXL_PROFILE_SCOPE;

        if (!s_StaticQuadVertices.empty())
        {
            s_StaticQuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, static_cast<uint32_t>(s_StaticQuadVertices.size() * sizeof(QuadVertex)), s_StaticQuadVertices.data());
            s_StaticQuadIBO = GPUBuffer::Create(GPUBufferUsage::Index, static_cast<uint32_t>(s_StaticQuadIndices.size() * sizeof(uint32_t)), s_StaticQuadIndices.data());
        }

        if (!s_StaticCubeVertices.empty())
        {
            s_StaticCubeVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, static_cast<uint32_t>(s_StaticCubeVertices.size() * sizeof(CubeVertex)), s_StaticCubeVertices.data());
            s_StaticCubeIBO = GPUBuffer::Create(GPUBufferUsage::Index, static_cast<uint32_t>(s_StaticCubeIndices.size() * sizeof(uint32_t)), s_StaticCubeIndices.data());
        }

        const auto layout = QuadVertex::GetLayout();

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            s_StaticQuadVAO->AddVertexBuffer(s_StaticQuadVBO, layout);
            s_StaticQuadVAO->SetIndexBuffer(s_StaticQuadIBO);
        }
    }

    float Renderer::GetTextureIndex(const std::shared_ptr<Texture>& texture)
    {
        if (s_TextureUnitIndex >= k_MaxTextureUnits)
            Flush();

        float textureIndex = -1.0f;
        bool foundTexture = false;

        // Find the texture in texture storage
        for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
        {
            if (texture == s_TextureUnits[i])
            {
                textureIndex = static_cast<float>(i);
                foundTexture = true;
                break;
            }
        }

        // If the texture wasn't found, add it to the next texture unit
        if (!foundTexture)
        {
            textureIndex = static_cast<float>(s_TextureUnitIndex);
            s_TextureUnits[s_TextureUnitIndex] = texture;
            s_TextureUnitIndex++;
        }

        return textureIndex;
    }

    void Renderer::Flush()
    {
        PXL_PROFILE_SCOPE;

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            PXL_PROFILE_SCOPE_NAMED("Bind Textures");

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
            PXL_PROFILE_SCOPE_NAMED("Flush Dynamic Quads");

            PXL_ASSERT_MSG(s_QuadCamera, "Quad Camera isn't set");
            PXL_ASSERT_MSG(s_QuadPipeline, "Quad pipeline isn't set");

            s_QuadVBO->SetData(s_QuadCount * 4 * sizeof(QuadVertex), s_QuadVertices.data()); // THIS TAKES SIZE IN BYTES

            s_QuadBufferBindFunc();

            if (s_RendererAPIType == RendererAPIType::OpenGL)
                s_QuadPipeline->SetUniformData("u_Textures", UniformDataType::IntArray, k_MaxTextureUnits, s_Samplers.data());

            s_QuadPipeline->Bind();

            s_SetViewProjectionFunc(s_QuadPipeline, s_QuadCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_QuadCount * 6);

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.QuadCount += s_QuadCount;
            s_Stats.QuadVertexCount += s_QuadCount * 4;
            s_Stats.QuadIndexCount += s_QuadCount * 6;

            s_QuadCount = 0;
        }

        // Flush static quads if necessary
        if (!s_StaticQuadVertices.empty())
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Static Quads");

            PXL_ASSERT_MSG(s_StaticQuadVBO, "Static quad VBO is invalid, make sure you call StaticGeometryReady()");

            PXL_ASSERT_MSG(s_QuadCamera, "Quad camera isn't set");
            PXL_ASSERT_MSG(s_QuadPipeline, "Quad pipeline isn't set");
            PXL_ASSERT_MSG(s_StaticQuadIBO, "Static quad IBO is invalid");

            s_StaticQuadBindFunc();

            s_QuadPipeline->Bind();

            s_SetViewProjectionFunc(s_QuadPipeline, s_QuadCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(static_cast<uint32_t>(s_StaticQuadIndices.size()));

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.QuadCount += static_cast<uint32_t>(s_StaticQuadIndices.size()) / 3;
            s_Stats.QuadVertexCount += static_cast<uint32_t>(s_StaticQuadVertices.size());
            s_Stats.QuadIndexCount += static_cast<uint32_t>(s_StaticQuadIndices.size());
        }

        // Flush cubes if necessary
        if (s_CubeCount > 0)
        {
            PXL_ASSERT_MSG(s_CubeCamera, "Cube camera isn't set");
            PXL_ASSERT_MSG(s_CubePipeline, "Cube pipeline isn't set");

            s_CubeVBO->SetData(s_CubeCount * 24 * sizeof(CubeVertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES

            {
                PXL_PROFILE_SCOPE_NAMED("s_CubeBindFunc()");
                s_CubeBindFunc();
            }

            s_CubePipeline->Bind();

            s_SetViewProjectionFunc(s_CubePipeline, s_CubeCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_CubeCount * 36);

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.CubeCount += s_CubeCount;
            s_Stats.CubeVertexCount += s_CubeCount * 24;
            s_Stats.CubeIndexCount += s_CubeCount * 36;

            s_CubeCount = 0;
        }

        // Flush static cubes if necessary
        if (!s_StaticCubeVertices.empty())
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Static Cubes");

            PXL_ASSERT_MSG(s_StaticCubeVBO, "Static cube VBO is invalid, make sure you call StaticGeometryReady()");

            PXL_ASSERT_MSG(s_CubeCamera, "Cube camera isn't set");
            PXL_ASSERT_MSG(s_CubePipeline, "Cube pipeline isn't set");
            PXL_ASSERT_MSG(s_StaticCubeIBO, "Static cube IBO is invalid");

            // TODO: Move this into a StaticCubeBind lambda
            s_StaticCubeVBO->Bind();
            s_CubeIBO->Bind();

            s_CubePipeline->Bind();

            s_SetViewProjectionFunc(s_CubePipeline, s_CubeCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(static_cast<uint32_t>(s_StaticCubeIndices.size()));

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.CubeCount += static_cast<uint32_t>(s_StaticCubeIndices.size()) / 36;
            s_Stats.CubeVertexCount += static_cast<uint32_t>(s_StaticCubeVertices.size());
            s_Stats.CubeIndexCount += static_cast<uint32_t>(s_StaticCubeIndices.size());
        }

        // Flush lines if necessary
        if (s_LineCount > 0)
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Lines");

            PXL_ASSERT_MSG(s_LineCamera, "Line camera isn't set");
            PXL_ASSERT_MSG(s_LinePipeline, "Line pipeline isn't set");

            s_LineVBO->SetData(s_LineCount * 2 * sizeof(LineVertex), s_LineVertices.data());

            s_LineBindFunc();

            s_LinePipeline->Bind();

            s_SetViewProjectionFunc(s_LinePipeline, s_LineCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawLines(s_LineCount * 2);

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.LineCount += s_LineCount;
            s_Stats.LineVertexCount += s_LineCount * 2;

            s_LineCount = 0;
        }
    }

    glm::mat4 Renderer::CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        // clang-format off
        return glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) 
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
            * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, scale.z });
        // clang-format on
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