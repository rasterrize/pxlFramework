#pragma once

namespace pxl
{
    enum class GPUBufferUsage
    {
        None,
        Vertex,
        Index,
        Uniform,
    };

    enum class GPUBufferDrawHint
    {
        Static,
        Dynamic,
    };

    class GPUBuffer
    {
    public:
        virtual ~GPUBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetData(uint32_t size, const void* data) = 0;

        static std::shared_ptr<GPUBuffer> Create(GPUBufferUsage usage, GPUBufferDrawHint drawHint, uint32_t size, const void* data);
    };
}