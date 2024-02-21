#pragma once

#include "../VertexArray.h"

#include "../Buffer.h"

namespace pxl
{
    // Vulkan Vertex Arrays do nothing other than encapsulate VBO's and an IBO so binding them at draw time is simpler
    class VulkanVertexArray : public VertexArray
    {
    public:
        virtual ~VulkanVertexArray() = default;

        virtual void Bind() override { m_VertexBuffer->Bind(), m_IndexBuffer->Bind(); }
        virtual void Unbind() override {} // could technically call the unbind functions but they do nothing anyway for vulkan

        virtual void AddVertexBuffer(const std::shared_ptr<Buffer>& vertexBuffer, const BufferLayout& layout) override { m_VertexBuffer = vertexBuffer; } 
        virtual void SetIndexBuffer(const std::shared_ptr<Buffer>& indexBuffer) override { m_IndexBuffer = indexBuffer; }

        virtual std::shared_ptr<Buffer> GetVertexBuffer() override { return nullptr; }
        virtual std::shared_ptr<Buffer> GetIndexBuffer() override { return nullptr; }
    private:
        std::shared_ptr<Buffer> m_VertexBuffer;
        std::shared_ptr<Buffer> m_IndexBuffer;
    };
}