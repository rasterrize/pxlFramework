#include "Renderer.h"

#include <glm/gtc/quaternion.hpp>

#include "BufferLayout.h"
#include "GPUBuffer.h"
#include "UniformLayout.h"
#include "Utils/EnumStringHelper.h"
#include "Vertices.h"

namespace pxl
{
    Renderer::Renderer(const RendererConfig& config)
        : m_Config(config)
    {
        PXL_PROFILE_SCOPE;

        m_GraphicsAPI = GraphicsAPI::Create(m_Config.APIType);
        PXL_ASSERT_MSG(m_GraphicsAPI, "Failed to create GraphicsAPI for renderer");

        m_GraphicsContext = m_GraphicsAPI->CreateGraphicsContext();

        GraphicsDeviceSpecs deviceSpecs = {
            .Preference = GPUPreference::Discrete,
            .RendererConfig = config,
        };

        m_GraphicsDevice = m_GraphicsAPI->CreateGraphicsDevice(deviceSpecs);

        const std::vector<ColouredVertex> vertices = {
            {  { 0.5f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Vertex 1: Red
            { { 0.0f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Vertex 2: Green
            { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }  // Vertex 3: Blue
        };

        // Init shader manager
        ShaderManagerConfig shaderManagerConfig = {};
        shaderManagerConfig.ShaderDirectories.push_back(RendererConstants::k_FrameworkShaderDirectory);
        shaderManagerConfig.CacheDirectory = m_Config.ShaderCacheDirectory;
        shaderManagerConfig.UseCache = m_Config.UseShaderCache;

        m_ShaderManager = std::make_unique<ShaderManager>(shaderManagerConfig);

        // Add pxlFramework shaders to shader manager
        m_ShaderManager->Add({ "coloured_vertex.vert", ShaderStage::Vertex });
        m_ShaderManager->Add({ "coloured_vertex.frag", ShaderStage::Fragment });

        m_ShaderManager->Add({ "textured_vertex.vert", ShaderStage::Vertex });
        m_ShaderManager->Add({ "textured_vertex.frag", ShaderStage::Fragment });

        // Append user list of shaders to shader manager
        m_ShaderManager->Add(m_Config.UserShadersToCompile);

        // Compile shaders
        m_ShaderManager->CompileAll(m_GraphicsDevice);

#define PXL_DRAW_HELLO_TRIANGLE 1
#if PXL_DRAW_HELLO_TRIANGLE
        {
            const std::vector<ColouredVertex> vertices = {
                { { -1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Vertex 1: Red
                {  { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Vertex 2: Green
                { { 0.5f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }  // Vertex 3: Blue
            };

            GPUBufferSpecs triangleBufferSpecs = {};
            triangleBufferSpecs.Size = sizeof(ColouredVertex) * vertices.size();
            triangleBufferSpecs.DrawHint = GPUBufferDrawHint::Dynamic;
            triangleBufferSpecs.Usage = GPUBufferUsage::Vertex;
            triangleBufferSpecs.Data = vertices.data();

            m_TriangleBuffer = m_GraphicsDevice->CreateBuffer(triangleBufferSpecs);

            GraphicsPipelineSpecs trianglePipelineSpecs = {};
            trianglePipelineSpecs.BufferLayout = ColouredVertex::GetLayout();
            trianglePipelineSpecs.CullMode = CullMode::None;
            trianglePipelineSpecs.FrontFace = FrontFace::CounterClockwise;
            trianglePipelineSpecs.PolygonMode = PolygonMode::Fill;
            trianglePipelineSpecs.PrimitiveTopology = PrimitiveTopology::Triangle;
            trianglePipelineSpecs.Shaders[ShaderStage::Vertex] = m_ShaderManager->Get("coloured_vertex.vert");
            trianglePipelineSpecs.Shaders[ShaderStage::Fragment] = m_ShaderManager->Get("coloured_vertex.frag");

            UniformLayout uniformLayout = {};
            uniformLayout.Add({ "ubo", UniformDataType::Mat4, ShaderStage::Vertex, sizeof(glm::mat4) });
            trianglePipelineSpecs.UniformLayout = uniformLayout;

            m_TrianglePipeline = m_GraphicsDevice->CreateGraphicsPipeline(trianglePipelineSpecs);
        }
#endif

        // --------------------
        // Prepare Quad Data
        // --------------------
        {
            // Prepare quad indices
            std::vector<uint32_t> indices(m_Config.VerticesPerBatch * 1.5f);
            constexpr std::array<uint32_t, 6> defaultIndices = Quad::GetDefaultIndices();

            uint32_t offset;
            for (size_t i = 0; i < indices.size(); i += 6)
            {
                for (uint32_t j = 0; j < 6; j++)
                {
                    indices[i + j] = defaultIndices[j] + offset;
                }

                offset += 4;
            }

            // Prepare vertex batching
            m_QuadBatch = std::make_unique<VertexBatch<TexturedVertex>>(m_GraphicsDevice, m_Config.VerticesPerBatch);

            // Create index buffer
            GPUBufferSpecs bufferSpecs = {};
            bufferSpecs.Usage = GPUBufferUsage::Index;
            bufferSpecs.DrawHint = GPUBufferDrawHint::Dynamic;
            bufferSpecs.Size = indices.size() * sizeof(uint32_t);
            bufferSpecs.Data = indices.data();
            m_QuadIndexBuffer = m_GraphicsDevice->CreateBuffer(bufferSpecs);



            // Create graphics pipeline
            GraphicsPipelineSpecs quadPipelineSpecs = {};
            quadPipelineSpecs.BufferLayout = TexturedVertex::GetLayout();
            quadPipelineSpecs.CullMode = CullMode::None;
            quadPipelineSpecs.FrontFace = FrontFace::CounterClockwise;
            quadPipelineSpecs.PolygonMode = PolygonMode::Fill;
            quadPipelineSpecs.PrimitiveTopology = PrimitiveTopology::Triangle;
            quadPipelineSpecs.Shaders[ShaderStage::Vertex] = m_ShaderManager->Get("textured_vertex.vert");
            quadPipelineSpecs.Shaders[ShaderStage::Fragment] = m_ShaderManager->Get("textured_vertex.frag");

            m_QuadPipeline = m_GraphicsDevice->CreateGraphicsPipeline(quadPipelineSpecs);
        }

        // --------------------
        // Prepare Line Data
        // --------------------
        // {
        // const auto bufferLayout = LineVertex::GetLayout();

        // Prepare Buffers
        // m_LineVBO = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Dynamic, k_MaxLineVertexCount * sizeof(LineVertex), nullptr);

        // GraphicsPipelineSpecs pipelineSpecs;
        // pipelineSpecs.PrimitiveTopology = PrimitiveTopology::Line;
        // pipelineSpecs.BufferLayout = bufferLayout;
        // pipelineSpecs.Shaders[ShaderStage::Vertex] = ShaderManager::Get("line_ogl.vert");
        // pipelineSpecs.Shaders[ShaderStage::Fragment] = ShaderManager::Get("line_ogl.frag");

        // m_DefaultPipelines[RendererGeometryTarget::Line] = GraphicsPipeline::Create(pipelineSpecs);
        // }

        // --------------------
        // Prepare Mesh Data
        // --------------------
        // {
        //     const auto bufferLayout = MeshVertex::GetLayout();

        //     GraphicsPipelineSpecs pipelineSpecs;
        //     pipelineSpecs.PrimitiveTopology = PrimitiveTopology::Triangle;
        //     pipelineSpecs.BufferLayout = bufferLayout;
        //     pipelineSpecs.PolygonMode = PolygonMode::Fill;
        //     pipelineSpecs.CullMode = CullMode::Back;

        //     // m_DefaultPipelines[RendererGeometryTarget::Mesh] = GraphicsPipeline::Create(pipelineSpecs);
        // }

        // Prepare white pixel texture
        // std::vector<uint8_t> pixelBytes = { 0xff, 0xff, 0xff, 0xff };
        // Image image(pixelBytes, Size2D(1), ImageFormat::RGBA8);
        // m_WhitePixelTexture = Texture::Create(image, { .Filter = SampleFilter::Nearest });

        PXL_LOG_INFO(LogArea::Renderer, "Renderer initialized with {} graphics API", EnumStringHelper::ToString(m_Config.APIType));
    }

    Renderer::~Renderer()
    {
        // m_TextureHandler.reset();
        // m_WhitePixelTexture.reset();
        // m_QuadBatch.reset();

        m_GraphicsDevice->FreeResources();
        m_GraphicsDevice.reset();

        m_GraphicsContext.reset();

        m_GraphicsAPI.reset();

        PXL_LOG_INFO(LogArea::Renderer, "Renderer shutdown");
    }

    void Renderer::InitImGui()
    {
        m_ImGuiRenderer = m_GraphicsDevice->CreateImGuiRenderer(m_Config.Window);
    }

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;

        m_GraphicsContext->Begin(m_GraphicsDevice);

        if (m_ImGuiRenderer)
            m_ImGuiRenderer->NewFrame();
        
        // Set first texture unit as white pixel texture
        // m_TextureHandler->UseTexture(m_WhitePixelTexture);
    }

    void Renderer::End()
    {
        PXL_PROFILE_SCOPE;

        // Flush();

        // if (GUI::IsInitialized())
        // {
        //     GUI::Update();
        //     GUI::Render();
        // }

        // TODO TEMP
        DrawParams params;
        params.Pipeline = m_TrianglePipeline;
        params.VertexBuffers.push_back(m_TriangleBuffer);
        params.VertexCount = 3;
        m_GraphicsContext->Draw(params);

        // Draw ImGui data
        if (m_ImGuiRenderer)
            m_ImGuiRenderer->Render(m_GraphicsDevice);

        m_GraphicsContext->End(m_GraphicsDevice);

        m_GraphicsDevice->Present();

        ResetFrameStats();

        m_RendererStats.FrameCount++;
    }

    void Renderer::Submit(const Quad& quad)
    {
        PXL_PROFILE_SCOPE;

        if (m_QuadBatch->GetVertexSpaceLeft() < 4)
        {
            m_QuadBatch->UploadData();

            DrawParams params;
            params.Pipeline = m_QuadPipeline;
            params.VertexBuffers.push_back(m_QuadBatch->GetVertexBuffer());
            params.VertexCount = static_cast<uint32_t>(m_QuadBatch->GetVertices().size());
            m_GraphicsContext->DrawIndexed(params, m_QuadIndexBuffer);
        }

        std::array<glm::vec2, 4> texCoords = Quad::GetDefaultTexCoords();
        if (quad.TextureUV.has_value())
            texCoords = quad.TextureUV.value();

        std::array<TexturedVertex, 4> defaultVertices = Quad::GetDefaultVerticesWithOrigin(quad.Origin);
        glm::mat4 transform = CalculateTransform(quad.Position, quad.Rotation, glm::vec3(quad.Size, 1.0f));

        std::vector<TexturedVertex> vertices(4);
        for (uint32_t i = 0; i < 4; i++)
        {
            vertices[i] = {
                .Position = transform * glm::vec4(defaultVertices[i].Position, 1.0f),
                .Colour = quad.Colour,
                .TexCoords = texCoords[i],
                .TexIndex = 0,
            };
        }

        m_QuadBatch->AddVertices(vertices);
    }

    void Renderer::Submit(const Line& line)
    {
        PXL_PROFILE_SCOPE;

        // if (m_LineCount >= k_MaxLineCount)
        //     Flush();

        // const auto vertexCount = s_LineCount * 2;

        // glm::vec3 centerPos = (line.StartPosition + line.EndPosition) / 2.0f;

        // glm::mat4 transform = CalculateTransform(centerPos, line.Rotation, glm::vec3(1.0f));

        // s_LineVertices[vertexCount + 0] = {
        //     .Position = transform * glm::vec4(line.StartPosition, 1.0f),
        //     .Colour = line.Colour,
        // };
        // s_LineVertices[vertexCount + 1] = {
        //     .Position = transform * glm::vec4(line.EndPosition, 1.0f),
        //     .Colour = line.Colour,
        // };

        // s_LineCount++;
    }

    void Renderer::Submit(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        PXL_PROFILE_SCOPE;

        // if (!mesh)
        // {
        //     PXL_LOG_WARN(LogArea::Renderer, "Failed to draw mesh as it was invalid");
        //     return;
        // }

        // glm::quat rotationY = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::quat rotationZ = glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        // glm::quat rotationX = glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        // glm::quat rotationQuat = rotationY * rotationZ * rotationX;

        // glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), position);
        // glm::mat4 rotationMat = glm::mat4_cast(rotationQuat);
        // glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
        // glm::mat4 transform = translateMat * rotationMat * scaleMat;

        // meshPipeline->Bind();
        // meshPipeline->SetUniformData("u_Transform", UniformDataType::Mat4, &transform);

        // if (!s_MeshVBOs.contains(mesh))
        // {
        //     s_MeshVBOs[mesh] = GPUBuffer::Create(GPUBufferUsage::Vertex, GPUBufferDrawHint::Dynamic, static_cast<uint32_t>(mesh->Vertices.size() * sizeof(MeshVertex)), mesh->Vertices.data());
        //     s_MeshIBOs[mesh] = GPUBuffer::Create(GPUBufferUsage::Index, GPUBufferDrawHint::Static, static_cast<uint32_t>(mesh->Indices.size() * sizeof(uint32_t)), mesh->Indices.data());

        //     if (s_RendererAPIType == RendererAPIType::OpenGL)
        //     {
        //         s_MeshVAOs[mesh] = VertexArray::Create();
        //         s_MeshVAOs[mesh]->AddVertexBuffer(s_MeshVBOs[mesh], MeshVertex::GetLayout());
        //         s_MeshVAOs[mesh]->SetIndexBuffer(s_MeshIBOs[mesh]);
        //     }
        // }

        // if (s_RendererAPIType == RendererAPIType::OpenGL)
        // {
        //     s_MeshVAOs[mesh]->Bind();
        // }
        // else
        // {
        //     s_MeshVBOs[mesh]->Bind();
        //     s_MeshIBOs[mesh]->Bind();
        // }

        // auto vp = m_Camera->GetViewProjectionMatrix();
        // meshPipeline->SetUniformData("u_VP", UniformDataType::Mat4, &vp);

        // m_GraphicsAPI->DrawIndexed(static_cast<uint32_t>(mesh->Indices.size()));

        // m_FrameStats.PipelineBinds++;
        // m_FrameStats.DrawCalls++;
    }

    void Renderer::Flush()
    {
        PXL_PROFILE_SCOPE;

        // m_TextureHandler->BindTextures();

        // Flush quads if necessary

        // m_TextureHandler->UploadShaderData(m_QuadPipeline);

        // auto vp = m_Camera->GetViewProjectionMatrix();
        // m_QuadPipeline->SetUniformData("u_VP", UniformDataType::Mat4, &vp);

        // m_GraphicsAPI->DrawIndexed(m_QuadBatch->GetVertices().size() / 4 * 6);

        // m_FrameStats.PipelineBinds++;
        // m_FrameStats.DrawCalls++;

        // Flush lines if necessary
        // if (s_LineCount > 0)
        // {
        //     PXL_PROFILE_SCOPE_NAMED("Flush Lines");

        //     PXL_ASSERT_MSG(s_LineCamera, "Line camera isn't set");
        //     PXL_ASSERT_MSG(linePipeline, "Line pipeline isn't set");

        //     s_LineVBO->SetData(s_LineCount * 2 * sizeof(LineVertex), s_LineVertices.data());

        //     s_LineBindFunc();

        //     linePipeline->Bind();

        //     s_SetViewProjectionFunc(linePipeline, s_LineCamera->GetViewProjectionMatrix());

        //     s_RendererAPI->DrawLines(s_LineCount * 2);

        //     m_FrameStats.PipelineBinds++;
        //     m_FrameStats.DrawCalls++;

        //     s_LineCount = 0;
        // }
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
}