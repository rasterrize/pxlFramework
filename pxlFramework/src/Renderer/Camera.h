#include "BaseCamera.h"

namespace pxl
{
    enum class CameraType
    {
        Orthographic, Perspective
    };

    class Camera
    {
    public:
        static void Init(CameraType type);
        static void Shutdown();

        //static BaseCamera& Get() { return *s_Camera; }

        static glm::vec3 GetPosition() { return s_Camera->GetPosition(); } // once again not api-agnostic
        static void GetRotation();

        static void SetPosition(glm::vec3 position) { s_Camera->SetPosition(position); }
        static void SetRotation();
    private:
        friend class Application;
        static void Update();

        static bool s_Enabled;
        static std::shared_ptr<BaseCamera> s_Camera;
    };
}