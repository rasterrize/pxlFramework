#include <pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        virtual ~TestApplication() override;

        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnImGuiRender() override;
    private:
        std::shared_ptr<pxl::Window> m_Window;
        std::shared_ptr<pxl::Camera> m_Camera;
        std::shared_ptr<pxl::Shader> m_Shader;
        std::shared_ptr<pxl::GraphicsPipeline> m_Pipeline;

        glm::vec4 m_ClearColour = glm::vec4(0.0f);

        std::function<void(pxl::WindowSpecs&)> m_OnStartFunc;
        std::function<void(float dt)> m_OnUpdateFunc;
        std::function<void()> m_OnRenderFunc;
        std::function<void()> m_OnImGuiRender;

        //std::shared_ptr<pxl::Shader> m_MeshShader; // bad name lol
        //std::shared_ptr<pxl::Shader> m_LineShader;
        
    };
}