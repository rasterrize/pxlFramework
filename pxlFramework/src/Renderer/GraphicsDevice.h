#pragma once

#include "Core/Window.h"
#include "GPUBuffer.h"
#include "GraphicsPipeline.h"
#include "ImGuiRenderer.h"
#include "Shader.h"
#include "Texture.h"

namespace pxl
{
    enum class QueueType
    {
        Graphics,
        Compute,
    };

    enum class GPUType
    {
        Discrete,
        Integrated,
    };

    struct GraphicsDeviceSpecs
    {
        /// @brief If specified, selects the GPU by indexing into the available GPUs, and ignores TypePreference.
        int32_t DeviceIndex = -1;

        /// @brief Ignored if DeviceIndex is specified. Selects a suitable GPU based on a GPU type preference.
        GPUType TypePreference;

        /// @brief Window to associate the internal swapchain to.
        std::shared_ptr<Window> Window = nullptr;

        /// @brief Whether to enable vertical synchronization or not.
        bool VerticalSync = true;

        /// @brief Whether to triple buffering on the swapchain, setting the desired number swapchain images to at least 3.
        bool TripleBuffering = true;
    };

    struct GraphicsDeviceStats
    {
        uint32_t BufferCount;
        uint32_t ShaderCount;
        uint32_t GraphicsPipelineCount;
        uint32_t TextureCount;
        uint32_t AllocatedBytes;
    };

    struct GraphicsDeviceLimits
    {
        float MaxAnisotropicLevel = 0.0f;
    };

    /// @brief Represents a GPU (Graphics Processing Unit) used for allocating GPU resources and
    /// communicating with the window system.
    class GraphicsDevice
    {
    public:
        GraphicsDevice(const GraphicsDeviceSpecs& specs)
            : m_Specs(specs)
        {
        }

        virtual ~GraphicsDevice() = default;

        /// @brief Creates a new GPUBuffer object used to host various kinds of data in memory on the GPU.
        /// @param specs The specifications for the GPUBuffer.
        /// @return A new GPUBuffer shared resource object.
        virtual std::shared_ptr<GPUBuffer> CreateBuffer(const GPUBufferSpecs& specs) = 0;

        /// @brief Creates a new Texture object used to host image data on the GPU and texturing geometry.
        /// @param specs The specifications for the Texture.
        /// @return A new Texture shared resource object.
        virtual std::shared_ptr<Texture> CreateTexture(const TextureSpecs& specs) = 0;

        /// @brief Creates a new Shader object used as a component of a pipeline.
        /// @param specs The specifications for the Shader.
        /// @return A new Shader shared resource object.
        virtual std::shared_ptr<Shader> CreateShader(const ShaderSpecs& specs) = 0;

        /// @brief Creates a new GraphicsPipeline used to process data during graphics operations.
        /// @param specs The specifications for the GraphicsPipeline.
        /// @return A new GraphicsPipeline shared resource object.
        virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) = 0;

        /// @brief Creates a new ImGuiRenderer backend for rendering ImGui.
        /// @param window The window associated with the renderer.
        /// @return A new ImGuiRenderer shared resource object.
        virtual std::shared_ptr<ImGuiRenderer> CreateImGuiRenderer(const std::shared_ptr<Window>& window) = 0;

        /// @brief Presents the next rendered image to the window.
        virtual void Present() = 0;

        /// @brief Frees/destroys all the resources allocated on this GraphicsDevice.
        virtual void FreeResources() = 0;

        virtual void OnWindowResize() = 0;

        virtual void SetVerticalSync(bool enable) = 0;

        virtual uint32_t GetSwapchainImageIndex() const = 0;

        virtual uint32_t GetSwapchainImageCount() const = 0;

        GraphicsDeviceSpecs GetSpecs() const { return m_Specs; }
        GraphicsDeviceLimits GetLimits() const { return m_Limits; }
        GraphicsDeviceStats GetStats() const { return m_Stats; }

        std::string GetGPUName() const { return m_GPUName; }
        std::string GetDriverInfo() const { return m_DriverInfo; }
        GPUType GetGPUType() const { return m_GPUType; }

    protected:
        GraphicsDeviceSpecs m_Specs = {};
        GraphicsDeviceLimits m_Limits = {};
        GraphicsDeviceStats m_Stats = {};

        std::string m_GPUName;
        std::string m_DriverInfo;
        GPUType m_GPUType;
    };

    namespace Utils
    {
        inline std::string ToString(QueueType type)
        {
            switch (type)
            {
                case QueueType::Graphics: return "Graphics";
                case QueueType::Compute:  return "Compute";
                default:                  return "Unknown";
            }
        }

        inline std::string ToString(GPUType type)
        {
            switch (type)
            {
                case GPUType::Discrete:   return "Discrete";
                case GPUType::Integrated: return "Integrated";
                default:                  return "Unknown";
            }
        }
    }
}