#include <pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        ~TestApplication();

        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnImGuiRender() override;
    private:
        std::shared_ptr<pxl::Window> m_Window;
        std::shared_ptr<pxl::Camera> m_Camera;

        float m_NextCameraFOV = 0.0f;

        std::shared_ptr<pxl::Shader> m_Shader;

        std::shared_ptr<pxl::Shader> m_MeshShader;
        std::shared_ptr<pxl::Shader> m_LineShader;

        std::shared_ptr<pxl::Mesh> m_Mesh;
        
        glm::vec3 m_CameraPosition = glm::vec3(0.0f);
        glm::vec3 m_CameraRotation = glm::vec3(0.0f);
        glm::vec4 m_ClearColour = glm::vec4(0.0f);

        glm::vec2 m_LastCursorPosition = glm::vec2(0.0f);
        glm::vec2 m_MouseDelta = glm::vec2(0.0f);

        bool controllingCamera = true;

        glm::vec4 m_QuadColour = { 0.180f, 0.293f, 0.819f, 1.0f };

        glm::vec3 m_PlayerPosition = { 0.0f, 0.75f, 0.0f };

        pxl::WindowMode m_WindowMode;

        uint32_t m_TexIndex[2] = { 3, 2 };

        std::vector<std::shared_ptr<pxl::Texture2D>> m_TextureLibrary;
        std::vector<std::string> m_AudioLibrary;
    };
}