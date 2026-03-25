#pragma once

#include "GPUBuffer.h"
#include "GraphicsPipeline.h"
#include "RendererConfig.h"
#include "Shader.h"
#include "Texture.h"
#include "ImGuiRenderer.h"

namespace pxl
{
    struct GraphicsDeviceLimits
    {
        uint32_t MaxTextureSlots = 32;
        float VRAMLimit = 128.0f; // VRAM limit in megabytes
        float MaxAnisotropicLevel = 1.0f;
    };

    enum class QueueType
    {
        Graphics,
        Compute,
    };

    enum class GPUPreference
    {
        Discrete,
        Integrated,
    };

    struct GraphicsDeviceSpecs
    {
        GPUPreference Preference;
        RendererConfig RendererConfig;
    };

    /// @brief Represents a GPU (Graphics Processing Unit) used for allocating GPU resources and
    ///        communicating with the window system
    class GraphicsDevice
    {
    public:
        virtual ~GraphicsDevice() = default;

        virtual std::shared_ptr<GPUBuffer> CreateBuffer(const GPUBufferSpecs& specs) = 0;
        virtual std::shared_ptr<Texture> CreateTexture(const TextureSpecs& specs) = 0;
        virtual std::shared_ptr<Shader> CreateShader(const ShaderSpecs& specs) = 0;
        virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) = 0;
        
        virtual std::shared_ptr<ImGuiRenderer> CreateImGuiRenderer(const std::shared_ptr<Window>& window) = 0;

        virtual void Present() = 0;

        virtual void WaitIdle() const = 0;
        virtual void QueueWaitIdle(QueueType queueType) const = 0;

        /// @brief Frees/destroys all the resources allocated on this GraphicsDevice
        virtual void FreeResources() = 0;

        // TODO
        /// @brief Frees/destroys a specific resource that has been allocated on this GraphicsDevice
        virtual void FreeResource(const std::shared_ptr<GPUResource>& resource) {}

        virtual GraphicsDeviceLimits GetLimits() const = 0;
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
    }
}