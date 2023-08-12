#pragma once

#include "../Core/Window.h"
#include "RendererAPIType.h"
#include "Shader.h"
#include "VertexArray.h"
#include "../Core/Math.h"
#include "RendererAPI.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    struct Vertex
    {
        float Position[3];
        float TexCoords[2];
    };

    struct Mesh
    {
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;

        glm::mat4 Transform = glm::mat4(1.0f);

        void Translate(float x, float y, float z)
        {
            Transform = glm::translate(glm::mat4(1.0f), { x, y, z });
        }
    };

    class Renderer
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }

        static const RendererAPIType GetAPIType() { return s_RendererAPIType; }
        static std::shared_ptr<Shader> GetShader() { return s_RendererAPI->GetShader(); }

        static const float GetFPS() { return s_FPS; }
        static const float GetFrameTimeMS() { return 1 / s_FPS * 1000.0f; }

        static void Clear();
        static void SetClearColour(const vec4& colour);

        static void DrawArrays(int count);
        static void DrawLines(int count);
        static void DrawIndexed();
        
        static void Submit(const std::shared_ptr<Mesh>& mesh);
        static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
        static void Submit(const std::shared_ptr<Shader>& shader);
    private:
        static void CalculateFPS();
    private:
        friend class WindowGLFW;

        static bool s_Enabled;
        static RendererAPIType s_RendererAPIType;

        static std::unique_ptr<RendererAPI> s_RendererAPI;

        static float s_FPS;
        static uint16_t s_FrameCount;
        static float s_TimeAtLastFrame;

        //static std::vector<Mesh> s_Meshes;
    };
}   