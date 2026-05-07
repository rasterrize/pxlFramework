#include "Renderer/Renderer.h"

#include <glm/gtc/quaternion.hpp>

#include "Core/Events/EventHandler.h"
#include "Renderer.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/GPUBuffer.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/UniformLayout.h"
#include "Renderer/Vertices.h"

using namespace std::literals;

namespace pxl
{
    Renderer::Renderer(const RendererConfig& config)
        : m_Config(config), m_PerFrameData(RendererConstants::MaxFramesInFlight)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT_MSG(m_Config.Window, "Renderer must be created with a valid Window");

        PXL_LOG_INFO("Renderer initializing with {} graphics api", Utils::ToString(m_Config.APIType));

        // Init graphics API
        m_GraphicsAPI = GraphicsAPI::Create(m_Config.APIType);
        PXL_ASSERT(m_GraphicsAPI);

        // Init graphics context
        m_GraphicsContext = m_GraphicsAPI->CreateGraphicsContext({ config.ClearColour });
        PXL_ASSERT(m_GraphicsContext);

        // Init graphics device
        GraphicsDeviceSpecs deviceSpecs = {
            .TypePreference = GPUType::Discrete,
            .Window = config.Window,
            .FramesInFlightCount = RendererConstants::MaxFramesInFlight,
            .VerticalSync = config.VerticalSync,
            .AllowTearing = config.AllowTearing,
            .TripleBuffering = config.TripleBuffering,
        };

        m_GraphicsDevice = m_GraphicsAPI->CreateGraphicsDevice(deviceSpecs);
        PXL_ASSERT(m_GraphicsDevice);

        PXL_CREATE_AND_REGISTER_HANDLER(m_WindowFBResizeHandler, WindowFBResizeEvent, OnWindowFBResize);

        // Init shader manager
        ShaderManagerConfig shaderManagerConfig = {};
        shaderManagerConfig.ShaderDirectories.push_back(RendererConstants::FrameworkShaderDirectory);
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
        m_ShaderManager->CompileAll(*m_GraphicsDevice);

        // Init texture handler
        m_TextureHandler = m_GraphicsDevice->CreateTextureHandler();

        {
            // --------------------
            // Prepare Quad Data
            // --------------------

            // Prepare quad indices
            std::vector<uint32_t> indices(static_cast<uint32_t>(m_Config.VerticesPerBatch * 1.5f));
            constexpr std::array<uint32_t, 6> defaultIndices = Quad::GetDefaultIndices();

            uint32_t offset = 0;
            for (size_t i = 0; i < indices.size(); i += RendererConstants::IndicesPerQuad)
            {
                for (uint32_t j = 0; j < RendererConstants::IndicesPerQuad; j++)
                {
                    indices[i + j] = defaultIndices[j] + offset;
                }

                offset += RendererConstants::VerticesPerQuad;
            }

            // Prepare vertex batching
            for (auto& data : m_PerFrameData)
            {
                data.QuadBatch = std::make_unique<VertexBatch<TexturedVertex>>(*m_GraphicsDevice, m_Config.VerticesPerBatch);
            }

            // Create index buffer
            GPUBufferSpecs bufferSpecs = {};
            bufferSpecs.Usage = GPUBufferUsage::Index;
            bufferSpecs.DrawHint = GPUBufferDrawHint::Static;
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
            quadPipelineSpecs.TextureHandler = m_TextureHandler;

            UniformLayout uniformLayout = {};
            uniformLayout.Add({ "ubo", UniformDataType::Mat4, ShaderStage::Vertex, sizeof(glm::mat4) });
            quadPipelineSpecs.UniformLayout = uniformLayout;

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

        auto windowSize = config.Window->GetFramebufferSize().ToVec2();

        // Setup 2D camera
        OrthographicSettings orthoSettings = {};
        orthoSettings.CameraSettings.NearClip = -1.0f;
        orthoSettings.CameraSettings.FarClip = 1.0f;
        orthoSettings.Sides = { 0.0f, windowSize.x, 0.0f, windowSize.y };
        m_CameraUI = std::make_shared<pxl::OrthographicCamera>(orthoSettings);

        // Prepare white pixel texture
        std::vector<uint8_t> pixelBytes = { 0xff, 0xff, 0xff, 0xff };
        ImageMetadata metadata = { Size2D(1), ImageFormat::RGBA8 };
        auto image = std::make_shared<Image>(pixelBytes, metadata);

        TextureSpecs whitePixelSpecs = {};
        whitePixelSpecs.Image = image;
        whitePixelSpecs.Filter = SampleFilter::Nearest;

        m_WhitePixelTexture = m_GraphicsDevice->CreateTexture(whitePixelSpecs);

        m_TextureHandler->Add(m_WhitePixelTexture);

        if (m_Config.InitImGui)
            InitImGui();

        m_SleepTimer = std::make_unique<SleepTimer>();

        PXL_LOG_INFO("Renderer initialization finished");
    }

    Renderer::~Renderer()
    {
        m_GraphicsDevice->FreeResources();

        PXL_LOG_INFO("Renderer shutdown");
    }

    void Renderer::InitImGui()
    {
#ifdef PXL_ENABLE_IMGUI
        m_ImGuiRenderer = m_GraphicsDevice->CreateImGuiRenderer({ m_Config.Window });
#endif
    }

    void Renderer::Submit(Quad& quad)
    {
        Submit(quad, nullptr);
    }

    void Renderer::Submit(Quad& quad, const std::shared_ptr<Texture>& texture, const std::array<glm::vec2, 4>& texCoords)
    {
        PXL_PROFILE_SCOPE;

        // Wrap the quads rotation so we don't get any floating point imprecision
        Utils::WrapRotation(quad.Rotation);

        if (texture)
        {
            PXL_ASSERT(m_TextureHandler->GetIndex(texture) != UINT32_MAX);
        }

        auto q = Process(quad);

        auto& batch = *m_CurrentFrameData->QuadBatch;
        if (batch.GetVertexSpaceLeft() < RendererConstants::VerticesPerQuad)
            batch.Flush(*m_GraphicsDevice);

        // NOTE: Flip the rotation due to vulkan using inverted y coordinates
        auto transform = CalculateTransform(q.Position, glm::vec3(0, 0, -q.Rotation), { q.Size, 1.0f });

        auto vertices = Quad::GetDefaultVertices();

        uint32_t i = 0;
        for (auto& vertex : vertices)
        {
            vertex.Position = transform * glm::vec4(vertex.Position, 1.0f);
            vertex.Colour = q.Colour;
            vertex.TexCoords = texCoords.at(i);
            vertex.TexIndex = texture ? m_TextureHandler->GetIndex(texture) : 0;

            batch.AddVertex(vertex);
            i++;
        }
    }

    void Renderer::Submit(Quad& quad, const SubTexture& subTexture)
    {
        Submit(quad, subTexture.Texture.lock(), subTexture.Coords);
    }

    void Renderer::Submit(Quad& quad, AnimatedTexture& animatedTexture)
    {
        Submit(quad, animatedTexture.GetCurrentFrame());
    }

    void Renderer::Flush()
    {
        PXL_PROFILE_SCOPE;

        auto& quadBatch = *m_CurrentFrameData->QuadBatch;
        if (quadBatch.CanFlush())
        {
            auto vertexCount = quadBatch.UploadData();

            DrawParams params;
            params.Pipeline = m_QuadPipeline;
            params.VertexBuffer = quadBatch.GetCurrentVertexBuffer();
            params.UniformBuffer = m_CurrentFrameData->UniformBuffer;
            m_GraphicsContext->DrawIndexed(params, *m_QuadIndexBuffer, static_cast<uint32_t>(vertexCount * 1.5f));
        }
    }

    void Renderer::Begin()
    {
        PXL_PROFILE_SCOPE;

        Stopwatch sw;
        m_GraphicsDevice->WaitOnFrame(m_FrameInFlightIndex);
        m_FrameStats.GraphicsDeviceWaitTime = sw.GetElapsedMilliSec();

        m_BeginPoint = std::chrono::steady_clock::now();

        m_GraphicsContext->BeginFrame(*m_GraphicsDevice, m_FrameInFlightIndex);

        if (m_TextureHandler->NeedsUpload())
            m_TextureHandler->Upload();

        if (m_ImGuiRenderer)
            m_ImGuiRenderer->NewFrame();

        m_CurrentFrameData = &m_PerFrameData.at(m_FrameInFlightIndex);

        m_CurrentFrameData->QuadBatch->Reset();
    }

    void Renderer::End()
    {
        PXL_PROFILE_SCOPE;

        Flush();

        auto vp = m_CameraUI->GetViewProjectionMatrix();
        m_CurrentFrameData->UniformBuffer->SetData(sizeof(glm::mat4), 0, &vp);

#ifdef PXL_ENABLE_IMGUI
        // Draw ImGui data
        if (m_ImGuiRenderer)
            m_ImGuiRenderer->Render(*m_GraphicsDevice, m_FrameInFlightIndex);
#endif

        m_GraphicsContext->EndFrame(*m_GraphicsDevice);

        m_GraphicsDevice->Submit(m_FrameInFlightIndex);

        m_FrameStats.RenderTime = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_BeginPoint).count();

        m_GraphicsDevice->Present();

        if (m_FrameCount == 0 && m_Config.Window->WillShowOnceRendererIsWorking())
            m_Config.Window->Show();

        m_FrameStats.FrameInFlightIndex = m_FrameInFlightIndex;
        m_FrameStats.FrameCountIndex = m_FrameCount;

        m_FrameInFlightIndex = (m_FrameInFlightIndex + 1) % RendererConstants::MaxFramesInFlight;
        m_FrameCount++;
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

    void Renderer::SetTripleBuffering(bool value)
    {
        m_Config.TripleBuffering = value;
        m_GraphicsDevice->SetTripleBuffering(value);
    }

    void Renderer::SetAllowTearing(bool value)
    {
        m_Config.AllowTearing = value;
        m_GraphicsDevice->SetAllowTearing(value);
    }

    void Renderer::SetFramerateMode(FramerateMode mode)
    {
        // If we ever add multi-threading to the renderer then this may need to be refactored out to a RenderThread class,
        // but for now this is fine

        if (mode == FramerateMode::AdaptiveSync)
            m_AdaptiveSyncFramerateLimit = Window::GetPrimaryMonitor().GetCurrentVideoMode().RefreshRate - 3;

        m_Config.FramerateMode = mode;
    }

    void Renderer::ReloadPipelines()
    {
        m_ShaderManager->ReloadAll();

        m_QuadPipeline->Recreate();
    }

    std::shared_ptr<Texture> Renderer::CreateTexture(TextureSpecs& specs)
    {
        // Override anisotropy level
        specs.AnisotropyLevel = specs.AnisotropyLevel < 1.0f ? m_Config.AnisotropicFilteringLevel : specs.AnisotropyLevel;
        auto texture = m_GraphicsDevice->CreateTexture(specs);
        m_TextureHandler->Add(texture);
        m_TextureHandler->Upload();
        return texture;
    }

    glm::mat4 Renderer::CalculateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        PXL_PROFILE_SCOPE;

        // clang-format off
        return glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) 
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
            * glm::scale(glm::mat4(1.0f), scale);
        // clang-format on
    }

    glm::vec2 Renderer::PositionOfAnchorOnRect(const RectF& rect, Anchor2D anchor)
    {
        PXL_PROFILE_SCOPE;

        switch (anchor)
        {
            case Anchor2D::Centre:       return glm::vec2(rect.Right / 2.0f, rect.Top / 2.0f);
            case Anchor2D::CentreLeft:   return glm::vec2(rect.Left, rect.Top / 2.0f);
            case Anchor2D::CentreRight:  return glm::vec2(rect.Right, rect.Top / 2.0f);
            case Anchor2D::TopLeft:      return glm::vec2(rect.Left, rect.Top);
            case Anchor2D::TopCentre:    return glm::vec2(rect.Right / 2.0f, rect.Top);
            case Anchor2D::TopRight:     return glm::vec2(rect.Right, rect.Top);
            case Anchor2D::BottomLeft:   return glm::vec2(rect.Left, rect.Bottom);
            case Anchor2D::BottomCentre: return glm::vec2(rect.Right / 2.0f, rect.Bottom);
            case Anchor2D::BottomRight:  return glm::vec2(rect.Right, rect.Bottom);
            default:                     return glm::vec2(0.0f);
        }
    }

    glm::vec2 Renderer::OffsetOfOriginOnQuad(const Quad&)
    {
        PXL_PROFILE_SCOPE;

        auto offset = glm::vec2(0);
        // TODO!!!!!!
        // switch (quad.Origin)
        // {
        //     case Origin2D::BottomLeft:
        //     {
        //         offset.x += quad.Size.x / 2.0f;
        //         offset.y += quad.Size.y / 2.0f;
        //         break;
        //     }
        //     case Origin2D::BottomCentre:
        //     {
        //         offset.y += quad.Size.y / 2.0f;
        //         break;
        //     }
        // }

        return offset;
    }

    Quad Renderer::Process(const Quad& quad)
    {
        PXL_PROFILE_SCOPE;

        auto q = quad;
        RectF fbRect = m_CameraUI->GetSides();
        glm::vec2 fbSize = { fbRect.Right, fbRect.Top };
        q.Position += glm::vec3(PositionOfAnchorOnRect(fbRect, q.Anchor), 0);

        // TODO: condense the code
        switch (q.Scaling)
        {
            case Scaling2D::RelativeX:    q.Size.x *= fbSize.x; break;
            case Scaling2D::RelativeY:    q.Size.y *= fbSize.y; break;
            case Scaling2D::RelativeBoth: q.Size *= fbSize; break;
            case Scaling2D::ScaleUpToFit:
            {
                auto qAspect = q.Size.x / q.Size.y;
                auto fbAspect = fbSize.x / fbSize.y;

                if (fbAspect < qAspect)
                {
                    // scale by x
                    q.Size *= fbSize.x / q.Size.x;
                }
                else
                {
                    // scale by y
                    q.Size *= fbSize.y / q.Size.y;
                }

                break;
            }
            case Scaling2D::ScaleDownToFit:
            {
                if (q.Size.x > fbSize.x)
                {
                    // scale by x
                    q.Size *= fbSize.x / q.Size.x;
                }
                else if (q.Size.y > fbSize.y)
                {
                    // scale by y
                    q.Size *= fbSize.y / q.Size.y;
                }

                break;
            }
            default: break;
        }

        // NOTE: must be done after scaling
        q.Position += glm::vec3(OffsetOfOriginOnQuad(q), 0);

        return q;
    }

    void Renderer::OnWindowFBResize(const WindowFBResizeEvent& e)
    {
        if (e.GetWindow() != m_Config.Window)
            return;

        if (e.GetSize().HasZero())
        {
            Suspend();
            return;
        }

        Unsuspend();

        m_GraphicsDevice->OnWindowFBResize(e);

        m_CameraUI->SetSides({ 0.0f, e.GetSize().ToVec2().x, 0.0f, e.GetSize().ToVec2().y });
    }

    void Renderer::LimitFramerateIfNecessary()
    {
        PXL_PROFILE_SCOPE;

        uint32_t framesPerSecond = 0;
        if (!m_Config.Window->IsFocused())
            framesPerSecond = m_Config.UnfocusedFramerateLimit;
        else if (m_Config.FramerateMode == FramerateMode::Custom)
            framesPerSecond = m_Config.CustomFramerateLimit;
        else if (m_Config.FramerateMode == FramerateMode::AdaptiveSync)
            framesPerSecond = m_AdaptiveSyncFramerateLimit;

        if (framesPerSecond == 0)
        {
            m_FrameStats.FramerateLimitWaitTime = 0.0;
            m_FrameStats.FramerateLimitSleepTime = 0.0;
            m_FrameStats.FramerateLimitSpinTime = 0.0;
            return;
        }

        Stopwatch waitSW;

        auto limitNS = std::chrono::nanoseconds(1s) / framesPerSecond;
        auto frameStartPoint = Application::Get().GetUpdateStartPoint();

        // Sleep to avoid wasting CPU cycles, but only if our sleep accuracy allows for it
        if (m_SleepTimer && limitNS > m_SleepTimer->GetAccuracy())
        {
            const auto tolerance = m_SleepTimer->GetAccuracy();
            auto timeSinceFrameStart = std::chrono::steady_clock::now() - frameStartPoint;
            auto sleepTime = std::max(limitNS - timeSinceFrameStart - tolerance, 0ns);
            if (sleepTime > 0ns)
            {
                PXL_PROFILE_SCOPE_NAMED("Framerate limiter sleep");
                Stopwatch sw;
                m_SleepTimer->Sleep(sleepTime.count());
                m_FrameStats.FramerateLimitSleepTime = sw.GetElapsedMilliSec();
            }
        }
        else
        {
            m_FrameStats.FramerateLimitSleepTime = 0.0;
        }

        // Spin for the remaining time
        {
            PXL_PROFILE_SCOPE_NAMED("Framerate limiter spin");
            Stopwatch sw;
            const auto tolerance = 150ns;
            while (std::chrono::steady_clock::now() - frameStartPoint < limitNS - tolerance)
            {
            }

            m_FrameStats.FramerateLimitSpinTime = sw.GetElapsedMilliSec();
        }

        m_FrameStats.FramerateLimitWaitTime = waitSW.GetElapsedMilliSec();
    }
}