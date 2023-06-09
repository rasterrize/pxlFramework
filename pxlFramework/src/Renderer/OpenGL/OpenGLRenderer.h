#pragma once

#include "../RendererAPI.h"

#include "OpenGLVertexBuffer.h"
#include "OpenGLVertexArray.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLShader.h" 

namespace pxl
{
    class OpenGLRenderer : public RendererAPI
    {
    public:
        OpenGLRenderer();
        
        virtual void Clear() override;
        virtual void SetClearColour(float r, float g, float b, float a) override;

        virtual void DrawArrays(int count) override;
        virtual void DrawLines(int count) override;
        virtual void DrawIndexed() override;

        virtual std::shared_ptr<VertexArray> const GetVertexArray() override { return s_VertexArray; }
        virtual std::shared_ptr<Shader> const GetShader() override           { return s_Shader; }

        virtual void SetVertexArray(std::shared_ptr<VertexArray> vertexArray) override { s_VertexArray.swap(vertexArray); s_VertexArray->Bind(); }
        virtual void SetShader(std::shared_ptr<Shader> shader) override                { s_Shader.swap(shader); s_Shader->Bind(); }
    private:
        void BatchGeometry();
    private:
        std::shared_ptr<VertexArray> s_VertexArray;
        std::shared_ptr<Shader> s_Shader;
    };
}