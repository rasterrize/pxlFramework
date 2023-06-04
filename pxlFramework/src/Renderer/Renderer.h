#pragma once

#include <GLFW/glfw3.h>

#include "../Core/Math.h"

#include "GraphicsContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"

#include "RendererAPI.h"

namespace pxl
{
    enum class RendererAPIType
    {
        None,
        OpenGL,
        Vulkan,
        DirectX11,
        DirectX12
    };

    struct Vertex
    {
        glm::vec3 Position;
        //glm::vec2 TexCoords;
    };

    class Renderer
    {
    public:
        static void Init(RendererAPIType api);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }

        static const RendererAPIType GetRendererAPIType() { return s_RendererAPIType; }
        static std::shared_ptr<Shader> GetShader() { return s_RendererAPI->GetShader(); }
        
        static void Clear();
        static void SetClearColour(const vec4& colour);

        static void DrawArrays(int count);
        static void DrawLines(int count);
        static void DrawIndexed();

        static void Submit(const std::shared_ptr<OpenGLVertexArray>& vertexArray);
        static void Submit(const std::shared_ptr<OpenGLShader>& shader);

    private:
        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;
        static GLFWwindow* s_WindowHandle; // Might need to change this with DX12

        static std::unique_ptr<RendererAPI> s_RendererAPI;

        //static std::vector<Mesh> s_Meshes;
    };
}   