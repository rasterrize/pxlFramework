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
        
        glm::vec3 m_CameraPosition = glm::vec3(0.0f);
        glm::vec3 m_CameraRotation = glm::vec3(0.0f);
        pxl::vec4 m_ClearColour = pxl::vec4(0.0f);

        glm::vec2 m_LastCursorPosition = glm::vec2(0.0f);
        glm::vec2 m_MouseDelta = glm::vec2(0.0f);
        float m_SensitivityH = 0.1f, m_SensitivityV = 0.1f;

        bool controllingCamera = true;

        pxl::WindowMode m_WindowMode;

        uint32_t m_TexIndex[2] = { 3, 2 };

        std::vector<std::shared_ptr<pxl::Texture>> m_TextureLibrary;
        std::vector<std::string> m_AudioLibrary;
    };
}