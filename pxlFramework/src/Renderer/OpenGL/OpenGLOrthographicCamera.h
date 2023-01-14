#include "../BaseCamera.h"
#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLOrthographicCamera : public BaseCamera
    {
    public:
        OpenGLOrthographicCamera();

        virtual void Update() override;

        //virtual void SetPosition(const glm::vec3& position) override;
        //virtual void SetRotation(float rotation);

    private:

        //glm::vec3 m_Position; // objects need a z axis to determine what is infront of what
        float m_Rotation = 0.0f; // float for rotation because it is in 2D space
    };
}