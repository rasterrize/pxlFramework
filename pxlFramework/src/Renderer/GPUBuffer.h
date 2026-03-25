#pragma once

#include "GPUResource.h"

namespace pxl
{
    enum class GPUBufferUsage
    {
        Vertex,
        Index,
        Uniform,
    };

    enum class GPUBufferDrawHint
    {
        Static,
        Dynamic,
    };

    struct GPUBufferSpecs
    {
        /// @brief The type of data used in the GPUBuffer.
        GPUBufferUsage Usage;

        /// @brief Hint for the underlying API. Set based on the frequency of data setting.
        GPUBufferDrawHint DrawHint;

        /// @brief Size of the GPUBuffer in bytes.
        uint64_t Size = 0;

        /// @brief (Optional) an offset to for the initial data in the GPUBuffer.
        uint64_t Offset = 0;

        /// @brief (Optional) Initial set of data to be uploaded to the GPUBuffer.
        const void* Data = nullptr;
    };

    /// @brief Represents a buffer of memory allocated on a GraphicsDevice.
    class GPUBuffer : public GPUResource
    {
    public:
        virtual ~GPUBuffer() = default;

        virtual void Free() override = 0;

        virtual void SetData(uint64_t size, uint64_t offset, const void* data) = 0;
    };

    namespace Utils
    {
        inline std::string ToString(GPUBufferUsage usage)
        {
            switch (usage)
            {
                case GPUBufferUsage::Vertex:  return "Vertex";
                case GPUBufferUsage::Index:   return "Index";
                case GPUBufferUsage::Uniform: return "Uniform";
                default:                      return "Unknown";
            }
        }

        inline std::string ToString(GPUBufferDrawHint hint)
        {
            switch (hint)
            {
                case GPUBufferDrawHint::Static:  return "Static";
                case GPUBufferDrawHint::Dynamic: return "Dynamic";
                default:                         return "Unknown";
            }
        }
    }
}