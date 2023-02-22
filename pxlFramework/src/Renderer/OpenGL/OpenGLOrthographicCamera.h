#include "../BaseCamera.h"

namespace pxl
{
    class OpenGLOrthographicCamera : public BaseCamera
    {
    public:
        OpenGLOrthographicCamera();

        virtual void Update() override;
    private:
        virtual void RecalculateProjection() override;
    };
}