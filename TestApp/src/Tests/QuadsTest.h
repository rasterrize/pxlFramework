#include <pxl.h>

namespace TestApp
{
    class QuadsTest
    {
    public:
        static void OnStart(const std::shared_ptr<pxl::Window>& window);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnClose();
    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::Camera> m_Camera;
        static std::shared_ptr<pxl::Shader> m_Shader; 
        static std::shared_ptr<pxl::GraphicsPipeline> m_Pipeline;
        static glm::vec4 m_ClearColour;

        static glm::vec3 m_CameraPosition;
        static glm::vec3 m_CameraRotation;

        static bool m_ControllingCamera;

        static glm::vec4 m_QuadColour;

        static glm::vec2 m_SelectedTile;

        static int m_BlueQuadAmount;
        static int m_OrangeQuadAmount;
    };
}