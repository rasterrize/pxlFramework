#include "BaseCamera.h"

namespace pxl
{
    class Camera
    {
    public:
        static void Init(CameraType type);
        static void Shutdown();

        static BaseCamera& GetBaseCamera() { if (s_Enabled); return *s_Camera; }

        static glm::vec3 GetPosition() { if (s_Enabled) { return s_Camera->GetPosition(); } return glm::vec3(0.0f); } // once again not api-agnostic // should return an error instead
        static glm::vec3 GetRotation() { if (s_Enabled){ return s_Camera->GetRotation(); } return glm::vec3(0.0f); }

        static void SetPosition(glm::vec3 position) { if (s_Enabled) s_Camera->SetPosition(position); }
        static void SetRotation(glm::vec3 rotation) { if (s_Enabled) s_Camera->SetRotation(rotation); }
    private:
        friend class Application;
        static void Update();

        static bool s_Enabled;
        static std::shared_ptr<BaseCamera> s_Camera;
    };
}