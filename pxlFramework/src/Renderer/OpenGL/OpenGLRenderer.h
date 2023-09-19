#pragma once

#include "../RendererAPI.h"

#include "OpenGLVertexArray.h"
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

        virtual std::shared_ptr<VertexArray> const GetVertexArray() override { return m_VertexArray; }
        virtual std::shared_ptr<Shader> const GetShader() override           { return m_Shader; }

        virtual void SetVertexArray(std::shared_ptr<VertexArray> vertexArray) override { m_VertexArray.swap(vertexArray); m_VertexArray->Bind(); }
        virtual void SetShader(std::shared_ptr<Shader> shader) override                { m_Shader.swap(shader); m_Shader->Bind(); }
    private:
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Shader> m_Shader;
    };
}