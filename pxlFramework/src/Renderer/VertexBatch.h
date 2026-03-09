#pragma once

#include "GPUBuffer.h"
#include "Renderer/Renderer.h"

namespace pxl
{
    template<typename VertexType>
    class VertexBatch
    {
    public:
        VertexBatch(const std::unique_ptr<GraphicsDevice>& device, size_t maxVertexCount)
            : m_MaxVertexCount(maxVertexCount)
        {
            m_Vertices.reserve(maxVertexCount);

            GPUBufferSpecs specs;
            specs.Size = m_MaxVertexCount * sizeof(VertexType);
            specs.DrawHint = GPUBufferDrawHint::Dynamic;
            specs.Usage = GPUBufferUsage::Vertex;
            m_VertexBuffer = device->CreateBuffer(specs);
        }

        const std::vector<VertexType>& GetVertices() const { return m_Vertices; }
        std::shared_ptr<GPUBuffer> GetVertexBuffer() const { return m_VertexBuffer; }

        void AddVertices(const std::vector<VertexType>& vertices)
        {
            m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
        }

        size_t GetVertexSpaceLeft() const { return m_MaxVertexCount - m_Vertices.size(); }

        bool CanFlush() const { return !m_Vertices.empty(); }

        void UploadData()
        {
            m_VertexBuffer->SetData(m_Vertices.size() * sizeof(VertexType), 0, m_Vertices.data());

            m_Vertices.clear();
        }

    private:
        size_t m_MaxVertexCount;

        std::vector<VertexType> m_Vertices;

        std::shared_ptr<GPUBuffer> m_VertexBuffer = nullptr;
    };
}