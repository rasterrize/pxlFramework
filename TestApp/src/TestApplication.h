#include <pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        ~TestApplication();

        virtual void OnUpdate(float ts) override;
        virtual void OnImGuiRender() override;
    private:
        std::shared_ptr<pxl::Window> m_Window;

        std::shared_ptr<pxl::OpenGLVertexArray> m_VAO;
        std::shared_ptr<pxl::VertexBuffer> m_VBO;
        std::shared_ptr<pxl::IndexBuffer> m_IBO;
        std::shared_ptr<pxl::OpenGLShader> m_Shader;
        
        glm::vec3 m_CameraPosition;
        pxl::vec4 m_ClearColour = pxl::vec4(1.0f);

        glm::vec2 m_LastCursorPos = glm::vec2(1.0f);
    };
}