#pragma once

#include "Device.h"

namespace pxl
{
    enum class BufferUsage
    {
        None, Vertex, Index
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetData(uint32_t size, const void* data) = 0;
    
        static std::shared_ptr<VertexBuffer> Create(uint32_t size, const void* data);
        static std::shared_ptr<VertexBuffer> Create(std::shared_ptr<Device>& device, uint32_t size, const void* data);
        static std::shared_ptr<VertexBuffer> Create(uint32_t size);
    };
}