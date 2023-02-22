#include "BaseCamera.h"

namespace pxl
{
    class Camera
    {
    public:
        static void Init(CameraType type);
        static void Shutdown();

        static BaseCamera& GetBaseCamera() { return *s_Camera; }

        static glm::vec3 GetPosition() { if (s_Enabled); return s_Camera->GetPosition(); } // once again not api-agnostic // should return an error instead
        static glm::vec3 GetRotation() { if (s_Enabled); return s_Camera->GetRotation(); }

        static void SetPosition(glm::vec3 position) { if (s_Enabled) s_Camera->SetPosition(position); }
        static void SetRotation(glm::vec3 rotation) { if (s_Enabled) s_Camera->SetRotation(rotation); }
    private:
        friend class Application;
        static void Update();

        static bool s_Enabled;
        static std::shared_ptr<BaseCamera> s_Camera;
    };
}