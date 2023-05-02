#pragma once

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h" 

namespace pxl
{
    class RendererAPI
    {
    public:
        virtual void Clear() = 0;
        virtual void SetClearColour(float r, float g, float b, float a) = 0;

        virtual void DrawArrays(int count) = 0;
        virtual void DrawLines(int count) = 0;
        virtual void DrawIndexed() = 0;

        std::shared_ptr<VertexBuffer> GetVertexBuffer() { return s_VertexBuffer; }
        std::shared_ptr<VertexArray> GetVertexArray()   { return s_VertexArray; }
        std::shared_ptr<IndexBuffer> GetIndexBuffer()   { return s_IndexBuffer; }
        std::shared_ptr<Shader> GetShader()             { return s_Shader; }

        void SetVertexBuffer(OpenGLVertexBuffer* vertexBuffer) { s_VertexBuffer.reset(vertexBuffer); s_VertexBuffer->Bind(); }
        void SetVertexArray(OpenGLVertexArray* vertexArray)    { s_VertexArray.reset(vertexArray); s_VertexArray->Bind(); }
        void SetIndexBuffer(OpenGLIndexBuffer* indexBuffer)    { s_IndexBuffer.reset(indexBuffer); s_IndexBuffer->Bind(); }
        void SetShader(OpenGLShader* shader)                   { s_Shader.reset(shader); s_Shader->Bind(); }

        // SetVertexBuffer(VulkanVertexBuffer* vertexBuffer)
    protected:
        std::shared_ptr<VertexBuffer> s_VertexBuffer;
        std::shared_ptr<VertexArray> s_VertexArray;
        std::shared_ptr<IndexBuffer> s_IndexBuffer;
        std::shared_ptr<Shader> s_Shader;
    };
}