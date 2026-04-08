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

            m_GPUBufferSpecs.Size = m_MaxVertexCount * sizeof(VertexType);
            m_GPUBufferSpecs.DrawHint = GPUBufferDrawHint::Dynamic;
            m_GPUBufferSpecs.Usage = GPUBufferUsage::Vertex;

            m_VertexBuffers.push_back(device->CreateBuffer(m_GPUBufferSpecs));
        }

        const std::vector<VertexType>& GetVertices() const { return m_Vertices; }

        std::shared_ptr<GPUBuffer> GetCurrentVertexBuffer() const { return m_VertexBuffers.at(m_VertexBufferIndex); }

        void AddVertices(const std::vector<VertexType>& vertices)
        {
            m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
        }

        size_t GetVertexSpaceLeft() const { return m_MaxVertexCount - m_Vertices.size(); }

        bool CanFlush() const { return !m_Vertices.empty(); }

        void Reset()
        {
            m_Vertices.clear();

            m_VertexBufferIndex = 0;
        }

        void UploadData()
        {
            m_VertexBuffers.at(m_VertexBufferIndex)->SetData(m_Vertices.size() * sizeof(VertexType), 0, m_Vertices.data());

            m_Vertices.clear();
        }

        void NextVertexBuffer(const std::unique_ptr<GraphicsDevice>& device)
        {
            uint32_t nextIndex = m_VertexBufferIndex + 1;
            if (m_VertexBuffers.size() < nextIndex)
            {
                m_VertexBuffers.push_back(device->CreateBuffer(m_GPUBufferSpecs));
            }

            m_VertexBufferIndex++;
        }

    private:
        size_t m_MaxVertexCount = 0;
        std::vector<VertexType> m_Vertices;

        uint32_t m_VertexBufferIndex = 0;
        std::vector<std::shared_ptr<GPUBuffer>> m_VertexBuffers;
        GPUBufferSpecs m_GPUBufferSpecs = {};
    };
}