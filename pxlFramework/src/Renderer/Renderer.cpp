#include "Renderer.h"

#include <glm/gtc/quaternion.hpp>

#include "BufferLayout.h"
#include "GPUBuffer.h"
#include "UniformLayout.h"
#include "Vertices.h"

namespace pxl
{
    Renderer::Renderer(const RendererConfig& config)
        : m_Config(config), m_PerFrameData(RendererConstants::k_MaxFramesInFlight)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT_MSG(m_Config.Window, "Renderer must be created with a valid Window");

        // Init graphics API
        m_GraphicsAPI = GraphicsAPI::Create(m_Config.APIType);
        PXL_ASSERT(m_GraphicsAPI);

        // Init graphics context
        m_GraphicsContext = m_GraphicsAPI->CreateGraphicsContext({ config.ClearColour });
        PXL_ASSERT(m_GraphicsContext);

        // Set initial clear colour
        m_GraphicsContext->SetClearColour(config.ClearColour);

        // Init graphics device
        GraphicsDeviceSpecs deviceSpecs = {
            .TypePreference = GPUType::Discrete,
            .Window = config.Window,
            .FramesInFlightCount = RendererConstants::k_MaxFramesInFlight,
            .VerticalSync = config.VerticalSync,
            .TripleBuffering = config.TripleBuffering,
        };

        m_GraphicsDevice = m_GraphicsAPI->CreateGraphicsDevice(deviceSpecs);
        PXL_ASSERT(m_GraphicsDevice);

        PXL_CREATE_AND_REGISTER_HANDLER(m_WindowFBResizeHandler, WindowFBResizeEvent, OnWindowFBResize);

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

        {
            // --------------------
            // Prepare Quad Data
            // --------------------

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
            for (auto& data : m_PerFrameData)
            {
                data.QuadBatch = std::make_unique<VertexBatch<TexturedVertex>>(m_GraphicsDevice, m_Config.VerticesPerBatch);
            }

            // Create index buffer
            GPUBufferSpecs bufferSpecs = {};
            bufferSpecs.Usage = GPUBufferUsage::Index;
            bufferSpecs.DrawHint = GPUBufferDrawHint::Static;
            bufferSpecs.Size = indices.size() * sizeof(uint32_t);
            bufferSpecs.Data = indices.data();
            m_QuadIndexBuffer = m_GraphicsDevice->CreateBuffer(bufferSpecs);

            PerspectiveSettings settings = {};
            m_Camera3D = pxl::Camera::Create(settings);
            m_Camera3D->SetPosition({ 0.0f, 0.0f, 5.0f });

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

        // Create uniform buffers
        GPUBufferSpecs uboSpecs = {};
        uboSpecs.Usage = GPUBufferUsage::Uniform;
        uboSpecs.DrawHint = GPUBufferDrawHint::Dynamic;
        uboSpecs.Size = sizeof(glm::mat4);

        for (auto& data : m_PerFrameData)
        {
            data.UniformBuffer = m_GraphicsDevice->CreateBuffer(uboSpecs);
        }

        // Prepare white pixel texture
        // std::vector<uint8_t> pixelBytes = { 0xff, 0xff, 0xff, 0xff };
        // Image image(pixelBytes, Size2D(1), ImageFormat::RGBA8);
        // m_WhitePixelTexture = Texture::Create(image, { .Filter = SampleFilter::Nearest });

        if (m_Config.InitImGui)
            InitImGui();

        PXL_LOG_INFO(LogArea::Renderer, "Renderer initialized with {} graphics API", Utils::ToString(m_Config.APIType));
    }

    Renderer::~Renderer()
    {
        m_GraphicsDevice->FreeResources();

        PXL_LOG_INFO(LogArea::Renderer, "Renderer shutdown");
    }

    void Renderer::InitImGui()
    {
#ifdef PXL_ENABLE_IMGUI
        m_ImGuiRenderer = m_GraphicsDevice->CreateImGuiRenderer({ m_Config.Window });
#endif
    }

    void Renderer::Submit(const Quad& quad)
    {
        PXL_PROFILE_SCOPE;
    }

    void Renderer::Submit(const Line& line)
    {
        PXL_PROFILE_SCOPE;
    }

    void Renderer::Submit(const std::shared_ptr<Mesh>& mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        PXL_PROFILE_SCOPE;

        if (!mesh)
        {
            PXL_LOG_WARN(LogArea::Renderer, "Failed to draw mesh. Mesh is null");
            return;
        }
    }

    void Renderer::Flush()
    {
        PXL_PROFILE_SCOPE;
    }

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;

        m_GraphicsDevice->WaitOnFrame(m_FrameIndex);

        m_GraphicsContext->BeginFrame(m_GraphicsDevice, m_FrameIndex);

        if (m_ImGuiRenderer)
            m_ImGuiRenderer->NewFrame();

        // Set first texture unit as white pixel texture
        // m_TextureHandler->UseTexture(m_WhitePixelTexture);

        m_CurrentFrameData = &m_PerFrameData.at(m_FrameIndex);

        m_CurrentFrameData->QuadBatch->Reset();
    }

    void Renderer::End()
    {
        PXL_PROFILE_SCOPE;

        Flush();

        auto vp = m_Camera2D->GetViewProjectionMatrix();
        m_CurrentFrameData->UniformBuffer->SetData(sizeof(glm::mat4), 0, &vp);

#ifdef PXL_ENABLE_IMGUI
        // Draw ImGui data
        if (m_ImGuiRenderer)
            m_ImGuiRenderer->Render(m_GraphicsDevice, m_FrameIndex);
#endif

        m_GraphicsContext->EndFrame(m_GraphicsDevice);

        m_GraphicsDevice->Submit(*m_GraphicsContext, m_FrameIndex);

        m_GraphicsDevice->Present();

        m_FrameIndex = (m_FrameIndex + 1) % RendererConstants::k_MaxFramesInFlight;
    }

    void Renderer::SetClearColour(const glm::vec4& colour)
    {
        m_Config.ClearColour = colour;
        m_GraphicsContext->SetClearColour(colour);
    }

    void Renderer::SetVerticalSync(bool value)
    {
        m_Config.VerticalSync = value;
        m_GraphicsDevice->SetVerticalSync(value);
    }

    void Renderer::ReloadPipelines()
    {
        std::vector<std::shared_ptr<GraphicsPipeline>> pipelines;
        pipelines.push_back(m_TrianglePipeline);

        for (const auto& pipeline : pipelines)
        {
            auto specs = pipeline->GetSpecs();

            for (const auto& [stage, shader] : specs.Shaders)
            {
                if (m_ShaderManager->Reload(shader) != true)
                {
                    // Shader couldn't be reloaded continue
                    continue;
                }
            }

            pipeline->Recreate();
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

    RendererStats Renderer::GetStats() const
    {
        auto stats = m_RendererStats;

        if (m_GraphicsContext)
            stats.GraphicsContextStats = m_GraphicsContext->GetStats();

        if (m_GraphicsDevice)
            stats.GraphicsDeviceStats = m_GraphicsDevice->GetStats();

        return stats;
    }

    void Renderer::OnWindowFBResize(const WindowFBResizeEvent& e)
    {
        if (e.GetWindow() != m_Config.Window)
            return;

        m_GraphicsDevice->OnWindowResize();
    }
}