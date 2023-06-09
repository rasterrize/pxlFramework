#pragma once

#include "VertexArray.h"
#include "Shader.h"

namespace pxl
{
    struct Vertex
    {
        float Position[3];
        //glm::vec2 TexCoords;
    };

    class RendererAPI
    {
    public:
        virtual void Clear() = 0;
        virtual void SetClearColour(float r, float g, float b, float a) = 0;

        virtual void DrawArrays(int count) = 0;
        virtual void DrawLines(int count) = 0;
        virtual void DrawIndexed() = 0;

        virtual std::shared_ptr<VertexArray> const GetVertexArray() = 0;
        virtual std::shared_ptr<Shader> const GetShader() = 0;

        virtual void SetVertexArray(std::shared_ptr<VertexArray> vertexArray) = 0;
        virtual void SetShader(std::shared_ptr<Shader> shader) = 0;
    };
}