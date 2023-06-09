#pragma once

namespace pxl
{
    class IndexBuffer
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual const unsigned int GetCount() const = 0;

        virtual void SetData(int count, const void* data) = 0;
    protected:
        unsigned int m_Count;
    };
}