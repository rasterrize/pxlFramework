#pragma once

namespace pxl
{
    enum class BufferUsage
    {
        None, Vertex, Index, Uniform
    };

    class Buffer
    {
    public:
        virtual ~Buffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetData(uint32_t size, const void* data) = 0;
    
        static std::shared_ptr<Buffer> Create(BufferUsage usage, uint32_t size, const void* data);
        static std::shared_ptr<Buffer> Create(BufferUsage usage, uint32_t size);
    };
}