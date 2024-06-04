#include <pxl.h>

namespace TestApp
{
    class OGLVK
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnImGuiRender();
        static void OnClose();

    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::Shader> m_Shader;
        static std::shared_ptr<pxl::GraphicsPipeline> m_Pipeline;

        static std::shared_ptr<pxl::Camera> m_Camera;
    };
}