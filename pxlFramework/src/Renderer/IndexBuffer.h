#pragma once

namespace pxl
{
    class IndexBuffer
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        const unsigned int GetCount() const { return m_Count; }
    protected:
        IndexBuffer(unsigned int count) : m_Count(count) {};

        unsigned int m_Count;
    };
}