#include <pxl.h>

namespace TestApp
{
    class ModelViewer
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnClose();

    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::Camera> m_Camera;
        static std::shared_ptr<pxl::Shader> m_Shader; 
        static std::shared_ptr<pxl::GraphicsPipeline> m_Pipeline;
        static glm::vec4 m_ClearColour;

        static std::shared_ptr<pxl::Mesh> m_Mesh;
    };
}