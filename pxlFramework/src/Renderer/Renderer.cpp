#include "Renderer.h"

#include "BufferLayout.h"
#include "Core/Platform.h"
#include "Debug/GUI/GUI.h"
#include "GPUBuffer.h"
#include "OpenGL/OpenGLRenderer.h"
#include "ShaderManager.h"
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

    // General Data
    static std::function<void(const std::shared_ptr<GraphicsPipeline>&, const glm::mat4& vp)> s_SetViewProjectionFunc = nullptr;

    static std::unordered_map<RendererGeometryTarget, std::shared_ptr<GraphicsPipeline>> s_Pipelines;

    // Texture Data
    static uint32_t s_TextureUnitIndex = 0;

    static std::vector<std::shared_ptr<Texture>> s_TextureUnits;
    static std::vector<int32_t> s_Samplers;

    static std::shared_ptr<Texture> s_WhitePixelTexture = nullptr;

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

    // Line Data
    static uint32_t s_LineCount = 0;

    static std::array<LineVertex, k_MaxLineVertexCount> s_LineVertices;

    static std::shared_ptr<GPUBuffer> s_LineVBO = nullptr;

    static std::function<void()> s_LineBindFunc = nullptr;

    // Mesh Data
    static std::unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<GPUBuffer>> s_MeshVBOs;
    static std::unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<GPUBuffer>> s_MeshIBOs;
    static std::unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<VertexArray>> s_MeshVAOs;

    // For OpenGL
    static std::shared_ptr<VertexArray> s_QuadVAO = nullptr;
    static std::shared_ptr<VertexArray> s_CubeVAO = nullptr;
    static std::shared_ptr<VertexArray> s_LineVAO = nullptr;
    static std::shared_ptr<VertexArray> s_StaticQuadVAO = nullptr;

    void Renderer::Init(const std::shared_ptr<Window>& window)
    {
        PXL_PROFILE_SCOPE;

        if (s_Enabled)
        {
            PXL_LOG_WARN(LogArea::Renderer, "Renderer already initialized");
            // TODO: recreate all resources used for the renderer if the renderer api has changed
        }

        PXL_ASSERT_MSG(window->GetGraphicsContext(), "Window has no graphics context for renderer");

        s_ContextHandle = window->GetGraphicsContext();
        s_RendererAPIType = window->GetRendererAPI();

        // Evaluate renderer limits
        s_Limits = s_ContextHandle->GetLimits();
        s_TextureUnits.resize(s_Limits.MaxTextureUnits, nullptr);
        s_Samplers.resize(s_Limits.MaxTextureUnits);

        // Create renderer API object
        s_RendererAPI = RendererAPI::Create(s_RendererAPIType, window);

        PXL_ASSERT_MSG(s_RendererAPI, "Failed to create renderer api object");

        // Compile and create shaders
        switch (s_RendererAPIType)
        {
            case RendererAPIType::OpenGL:
                ShaderManager::LoadFromGLSL("resources/shaders/quad_textured_ogl.vert", ShaderStage::Vertex);
                ShaderManager::LoadFromGLSL("resources/shaders/quad_textured_ogl.frag", ShaderStage::Fragment);

                ShaderManager::LoadFromGLSL("resources/shaders/quad_ogl.vert", ShaderStage::Vertex);
                ShaderManager::LoadFromGLSL("resources/shaders/quad_ogl.frag", ShaderStage::Fragment);

                ShaderManager::LoadFromGLSL("resources/shaders/line_ogl.vert", ShaderStage::Vertex);
                ShaderManager::LoadFromGLSL("resources/shaders/line_ogl.frag", ShaderStage::Fragment);

                ShaderManager::LoadFromGLSL("resources/shaders/mesh_ogl.vert", ShaderStage::Vertex);
                break;

            case RendererAPIType::Vulkan:
                ShaderManager::LoadFromGLSL("resources/shaders/quad_vk.vert", ShaderStage::Vertex);
                ShaderManager::LoadFromGLSL("resources/shaders/quad_vk.frag", ShaderStage::Fragment);

                ShaderManager::LoadFromSPIRV("resources/shaders/compiled/quad_vert.spv", ShaderStage::Vertex);
                ShaderManager::LoadFromSPIRV("resources/shaders/compiled/quad_frag.spv", ShaderStage::Fragment);

                ShaderManager::LoadFromSPIRV("resources/shaders/compiled/line_vert.spv", ShaderStage::Vertex);
                ShaderManager::LoadFromSPIRV("resources/shaders/compiled/line_frag.spv", ShaderStage::Fragment);
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
            s_QuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Dynamic, k_MaxQuadVertexCount * sizeof(QuadVertex), nullptr);
            s_QuadIBO = GPUBuffer::Create(GPUBufferUsage::Index, GPUBufferDrawHint::Static, k_MaxQuadIndexCount * sizeof(uint32_t), s_QuadIndices.data());
            s_StaticQuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Static, k_MaxQuadVertexCount * sizeof(QuadVertex), nullptr);

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.PolygonMode = PolygonMode::Fill;
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

                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("quad_textured_ogl.vert");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("quad_textured_ogl.frag");
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

                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("quad_vk.vert");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("quad_vk.frag");

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_Pipelines[RendererGeometryTarget::Quad] = GraphicsPipeline::Create(pipelineSpecs);
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
            s_CubeVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Dynamic, k_MaxCubeVertexCount * sizeof(CubeVertex), nullptr);
            s_CubeIBO = GPUBuffer::Create(GPUBufferUsage::Index, GPUBufferDrawHint::Static, k_MaxCubeIndexCount * sizeof(uint32_t), s_CubeIndices.data());

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

                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("quad_ogl.vert");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("quad_ogl.frag");
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_CubeBindFunc = [&]()
                {
                    s_CubeVBO->Bind();
                    s_CubeIBO->Bind();
                };

                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("quad_vert.spv");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("quad_frag.spv");

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_Pipelines[RendererGeometryTarget::Cube] = GraphicsPipeline::Create(pipelineSpecs);
        }

        // --------------------
        // Prepare Line Data
        // --------------------
        {
            const auto bufferLayout = LineVertex::GetLayout();

            // Prepare Buffers
            s_LineVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Dynamic, k_MaxLineVertexCount * sizeof(LineVertex), nullptr);

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

                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("line_ogl.vert");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("line_ogl.frag");
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                s_LineBindFunc = [&]()
                {
                    s_LineVBO->Bind();
                };

                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("line_vert.spv");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("line_frag.spv");

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_Pipelines[RendererGeometryTarget::Line] = GraphicsPipeline::Create(pipelineSpecs);
        }

        // --------------------
        // Prepare Mesh Data
        // --------------------
        {
            const auto bufferLayout = MeshVertex::GetLayout();

            GraphicsPipelineSpecs pipelineSpecs;
            pipelineSpecs.PrimitiveType = PrimitiveTopology::Triangle;
            pipelineSpecs.VertexLayout = bufferLayout;
            pipelineSpecs.PolygonMode = PolygonMode::Fill;
            pipelineSpecs.CullMode = CullMode::Back;

            // Prepare other data based on renderer API
            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("mesh_ogl.vert");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("quad_ogl.frag");
            }
            else if (s_RendererAPIType == RendererAPIType::Vulkan)
            {
                pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("quad_vk.vert");
                pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("quad_vk.frag");

                PushConstantLayout pushConstantLayout;
                pushConstantLayout.Add({ "u_VP", UniformDataType::Mat4, ShaderStage::Vertex });
                // pushConstantLayout.Add({ "u_Transform", UniformDataType::Mat4, ShaderStage::Vertex });

                pipelineSpecs.PushConstantLayout = pushConstantLayout;
            }

            s_Pipelines[RendererGeometryTarget::Mesh] = GraphicsPipeline::Create(pipelineSpecs);
        }

        // Prepare white pixel texture
        std::vector<uint8_t> pixelBytes = { 0xff, 0xff, 0xff, 0xff };
        Image image(pixelBytes, Size2D(1), ImageFormat::RGBA8);
        s_WhitePixelTexture = Texture::Create(image, { .Filter = SampleFilter::Nearest });

        // Set samplers
        for (uint32_t i = 0; i < s_Limits.MaxTextureUnits; i++)
            s_Samplers[i] = i;

        // Set default cameras
        s_QuadCamera = pxl::Camera::Create(OrthographicSettings());
        s_CubeCamera = pxl::Camera::Create(PerspectiveSettings());
        s_LineCamera = pxl::Camera::Create(PerspectiveSettings());
        // s_MeshCamera = pxl::Camera::CreatePerspective({});

        PXL_LOG_INFO(LogArea::Renderer, "Finished preparing renderer");

        s_Enabled = true;
    }

    void Renderer::Shutdown()
    {
        if (!s_Enabled)
            return;

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
        s_RendererAPI->SetClearColour(Colour::AsVec4(colour));
    }

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;

        // NOTE: required atm otherwise application will call it even if the renderer isn't enabled
        if (!s_Enabled)
            return;

        s_RendererAPI->BeginFrame();

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

        s_RendererAPI->EndFrame();

        ResetStats();
    }

    void Renderer::SetCamera(RendererGeometryTarget target, const std::shared_ptr<Camera>& camera)
    {
        PXL_ASSERT(camera);

        switch (target)
        {
            case RendererGeometryTarget::Quad: s_QuadCamera = camera; return;
            case RendererGeometryTarget::Cube: s_CubeCamera = camera; return;
            case RendererGeometryTarget::Line: s_LineCamera = camera; return;
            case RendererGeometryTarget::Mesh: PXL_LOG_WARN(LogArea::Renderer, "Mesh cameras aren't supported yet"); return;
        }
    }

    void Renderer::SetCameraAll(const std::shared_ptr<Camera>& camera)
    {
        s_QuadCamera = camera;
        s_CubeCamera = camera;
        s_LineCamera = camera;
    }

    std::shared_ptr<GraphicsPipeline> Renderer::GetPipeline(RendererGeometryTarget target)
    {
        return s_Pipelines.at(target);
    }

    void Renderer::SetPipeline(RendererGeometryTarget target, const std::shared_ptr<GraphicsPipeline>& pipeline)
    {
        PXL_ASSERT(pipeline);

        s_Pipelines[target] = pipeline;
    }

    void Renderer::SetPipelineAll(const std::shared_ptr<GraphicsPipeline>& pipeline)
    {
        s_Pipelines[RendererGeometryTarget::Quad] = pipeline;
        s_Pipelines[RendererGeometryTarget::Cube] = pipeline;
        s_Pipelines[RendererGeometryTarget::Line] = pipeline;
        s_Pipelines[RendererGeometryTarget::Mesh] = pipeline;
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

        std::array<QuadVertex, 4> defaultVertices = Quad::GetDefaultVerticesWithOrigin(quad.Origin);

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

    void Renderer::AddLine(const Line& line)
    {
        PXL_PROFILE_SCOPE;

        if (s_LineCount >= k_MaxLineCount)
            Flush();

        const auto vertexCount = s_LineCount * 2;

        glm::vec3 centerPos = (line.StartPosition + line.EndPosition) / 2.0f;

        glm::mat4 transform = CalculateTransform(centerPos, line.Rotation, glm::vec3(1.0f));

        s_LineVertices[vertexCount + 0] = {
            .Position = transform * glm::vec4(line.StartPosition, 1.0f),
            .Colour = line.Colour,
        };
        s_LineVertices[vertexCount + 1] = {
            .Position = transform * glm::vec4(line.EndPosition, 1.0f),
            .Colour = line.Colour,
        };

        s_LineCount++;
    }

    void Renderer::AddLine(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec3& rotation, const glm::vec4& colour)
    {
        AddLine({ startPos, endPos, rotation, colour });
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

        auto meshPipeline = s_Pipelines.at(RendererGeometryTarget::Mesh);

        meshPipeline->Bind();
        meshPipeline->SetUniformData("u_Transform", UniformDataType::Mat4, &transform);

        if (!s_MeshVBOs.contains(mesh))
        {
            s_MeshVBOs[mesh] = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Dynamic, static_cast<uint32_t>(mesh->Vertices.size() * sizeof(MeshVertex)), mesh->Vertices.data());
            s_MeshIBOs[mesh] = GPUBuffer::Create(GPUBufferUsage::Index, GPUBufferDrawHint::Static, static_cast<uint32_t>(mesh->Indices.size() * sizeof(uint32_t)), mesh->Indices.data());

            if (s_RendererAPIType == RendererAPIType::OpenGL)
            {
                s_MeshVAOs[mesh] = VertexArray::Create();
                s_MeshVAOs[mesh]->AddVertexBuffer(s_MeshVBOs[mesh], MeshVertex::GetLayout());
                s_MeshVAOs[mesh]->SetIndexBuffer(s_MeshIBOs[mesh]);
            }
        }

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            s_MeshVAOs[mesh]->Bind();
        }
        else
        {
            s_MeshVBOs[mesh]->Bind();
            s_MeshIBOs[mesh]->Bind();
        }

        s_SetViewProjectionFunc(meshPipeline, s_QuadCamera->GetViewProjectionMatrix() * transform);

        s_RendererAPI->DrawIndexed(static_cast<uint32_t>(mesh->Indices.size()));

        s_Stats.PipelineBinds++;
        s_Stats.DrawCalls++;
        s_Stats.MeshCount++;
        s_Stats.MeshVertexCount += static_cast<uint32_t>(mesh->Vertices.size());
        s_Stats.MeshIndexCount += static_cast<uint32_t>(mesh->Indices.size());
    }

    void Renderer::ResetStaticGeometry(RendererGeometryTarget target)
    {
        switch (target)
        {
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
            s_StaticQuadVertices.emplace_back(
                transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                quad.Colour,
                defaultVertices[i].TexCoords,
                0.0f);
        }

        // Add indices
        for (uint32_t j = 0; j < 6; j++)
            s_StaticQuadIndices.emplace_back(s_StaticQuadIndexOffset + defaultIndices[j]);

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
            s_StaticCubeVertices.emplace_back(
                transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                cube.Colour,
                defaultVertices[i].TexCoords,
                0.0f);
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
            s_StaticQuadVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Static, static_cast<uint32_t>(s_StaticQuadVertices.size() * sizeof(QuadVertex)), s_StaticQuadVertices.data());
            s_StaticQuadIBO = GPUBuffer::Create(GPUBufferUsage::Index, GPUBufferDrawHint::Static, static_cast<uint32_t>(s_StaticQuadIndices.size() * sizeof(uint32_t)), s_StaticQuadIndices.data());
        }

        if (!s_StaticCubeVertices.empty())
        {
            s_StaticCubeVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Static, static_cast<uint32_t>(s_StaticCubeVertices.size() * sizeof(CubeVertex)), s_StaticCubeVertices.data());
            s_StaticCubeIBO = GPUBuffer::Create(GPUBufferUsage::Index, GPUBufferDrawHint::Static, static_cast<uint32_t>(s_StaticCubeIndices.size() * sizeof(uint32_t)), s_StaticCubeIndices.data());
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
        if (s_TextureUnitIndex >= s_Limits.MaxTextureUnits)
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

        // ---------------------
        // Prepare textures
        // ---------------------

        if (s_RendererAPIType == RendererAPIType::OpenGL)
        {
            PXL_PROFILE_SCOPE_NAMED("Bind Textures");

            for (uint32_t i = 0; i < s_TextureUnitIndex; i++)
            {
                s_TextureUnits[i]->Bind(i);
                s_Stats.TextureBinds++;
            }

            s_TextureUnitIndex = 0;
        }

        // ---------------------
        // Get pipelines
        // ---------------------

        auto quadPipeline = s_Pipelines.at(RendererGeometryTarget::Quad);
        auto cubePipeline = s_Pipelines.at(RendererGeometryTarget::Cube);
        auto linePipeline = s_Pipelines.at(RendererGeometryTarget::Line);
        auto meshPipeline = s_Pipelines.at(RendererGeometryTarget::Mesh);

        // ---------------------
        // Static Geometry
        // ---------------------

        // Flush static quads if necessary
        if (!s_StaticQuadVertices.empty())
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Static Quads");

            PXL_ASSERT_MSG(s_StaticQuadVBO, "Static quad VBO is invalid, make sure you call StaticGeometryReady()");

            PXL_ASSERT_MSG(s_QuadCamera, "Quad camera isn't set");
            PXL_ASSERT_MSG(quadPipeline, "Quad pipeline isn't set");
            PXL_ASSERT_MSG(s_StaticQuadIBO, "Static quad IBO is invalid");

            s_StaticQuadBindFunc();

            quadPipeline->Bind();

            s_SetViewProjectionFunc(quadPipeline, s_QuadCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(static_cast<uint32_t>(s_StaticQuadIndices.size()));

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.QuadCount += static_cast<uint32_t>(s_StaticQuadIndices.size()) / 3;
            s_Stats.QuadVertexCount += static_cast<uint32_t>(s_StaticQuadVertices.size());
            s_Stats.QuadIndexCount += static_cast<uint32_t>(s_StaticQuadIndices.size());
        }

        // Flush static cubes if necessary
        if (!s_StaticCubeVertices.empty())
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Static Cubes");

            PXL_ASSERT_MSG(s_StaticCubeVBO, "Static cube VBO is invalid, make sure you call StaticGeometryReady()");

            PXL_ASSERT_MSG(s_CubeCamera, "Cube camera isn't set");
            PXL_ASSERT_MSG(cubePipeline, "Cube pipeline isn't set");
            PXL_ASSERT_MSG(s_StaticCubeIBO, "Static cube IBO is invalid");

            // TODO: Move this into a StaticCubeBind lambda
            s_StaticCubeVBO->Bind();
            s_CubeIBO->Bind();

            cubePipeline->Bind();

            s_SetViewProjectionFunc(cubePipeline, s_CubeCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(static_cast<uint32_t>(s_StaticCubeIndices.size()));

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.CubeCount += static_cast<uint32_t>(s_StaticCubeIndices.size()) / 36;
            s_Stats.CubeVertexCount += static_cast<uint32_t>(s_StaticCubeVertices.size());
            s_Stats.CubeIndexCount += static_cast<uint32_t>(s_StaticCubeIndices.size());
        }

        // ---------------------
        // Dynamic Geometry
        // ---------------------

        // Flush quads if necessary
        if (s_QuadCount > 0)
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Dynamic Quads");

            PXL_ASSERT_MSG(s_QuadCamera, "Quad Camera isn't set");
            PXL_ASSERT_MSG(quadPipeline, "Quad pipeline isn't set");

            s_QuadVBO->SetData(s_QuadCount * 4 * sizeof(QuadVertex), s_QuadVertices.data()); // THIS TAKES SIZE IN BYTES

            s_QuadBufferBindFunc();

            // NOTE: Ensure we bind the pipeline before setting uniform data
            quadPipeline->Bind();

            if (s_RendererAPIType == RendererAPIType::OpenGL)
                quadPipeline->SetUniformData("u_Textures", UniformDataType::IntArray, s_Limits.MaxTextureUnits, s_Samplers.data());

            s_SetViewProjectionFunc(quadPipeline, s_QuadCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_QuadCount * 6);

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.QuadCount += s_QuadCount;
            s_Stats.QuadVertexCount += s_QuadCount * 4;
            s_Stats.QuadIndexCount += s_QuadCount * 6;

            s_QuadCount = 0;
        }

        // Flush cubes if necessary
        if (s_CubeCount > 0)
        {
            PXL_ASSERT_MSG(s_CubeCamera, "Cube camera isn't set");
            PXL_ASSERT_MSG(cubePipeline, "Cube pipeline isn't set");

            s_CubeVBO->SetData(s_CubeCount * 24 * sizeof(CubeVertex), s_CubeVertices.data()); // THIS TAKES SIZE IN BYTES

            {
                PXL_PROFILE_SCOPE_NAMED("s_CubeBindFunc()");
                s_CubeBindFunc();
            }

            cubePipeline->Bind();

            s_SetViewProjectionFunc(cubePipeline, s_CubeCamera->GetViewProjectionMatrix());

            s_RendererAPI->DrawIndexed(s_CubeCount * 36);

            s_Stats.PipelineBinds++;
            s_Stats.DrawCalls++;
            s_Stats.CubeCount += s_CubeCount;
            s_Stats.CubeVertexCount += s_CubeCount * 24;
            s_Stats.CubeIndexCount += s_CubeCount * 36;

            s_CubeCount = 0;
        }

        // Flush lines if necessary
        if (s_LineCount > 0)
        {
            PXL_PROFILE_SCOPE_NAMED("Flush Lines");

            PXL_ASSERT_MSG(s_LineCamera, "Line camera isn't set");
            PXL_ASSERT_MSG(linePipeline, "Line pipeline isn't set");

            s_LineVBO->SetData(s_LineCount * 2 * sizeof(LineVertex), s_LineVertices.data());

            s_LineBindFunc();

            linePipeline->Bind();

            s_SetViewProjectionFunc(linePipeline, s_LineCamera->GetViewProjectionMatrix());

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
        PXL_PROFILE_SCOPE;
        double currentTime = Platform::GetTime();
        s_Stats.FrameTime = static_cast<float>(currentTime - s_TimeAtLastFrame) * 1000.0f;
        s_Stats.FPS = 1000.0f / s_Stats.FrameTime;
        s_TimeAtLastFrame = currentTime;
    }
}