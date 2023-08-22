#include <pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        ~TestApplication();

        virtual void OnUpdate(float dt) override;
        virtual void OnImGuiRender() override;
    private:
        std::shared_ptr<pxl::Window> m_Window;
        std::shared_ptr<pxl::Camera> m_Camera;

        float m_NextCameraFOV = 0.0f;

        std::shared_ptr<pxl::VertexArray> m_VAO;
        std::shared_ptr<pxl::VertexBuffer> m_VBO;
        std::shared_ptr<pxl::IndexBuffer> m_IBO;
        std::shared_ptr<pxl::Shader> m_Shader;

        pxl::Mesh m_CubeMesh;
        glm::vec3 m_MeshPosition = glm::vec3(0.0f);
        
        glm::vec3 m_CameraPosition;
        pxl::vec4 m_ClearColour = pxl::vec4(1.0f);

        glm::vec2 m_LastCursorPos = glm::vec2(1.0f);

        pxl::WindowMode m_WindowMode;
    };
}