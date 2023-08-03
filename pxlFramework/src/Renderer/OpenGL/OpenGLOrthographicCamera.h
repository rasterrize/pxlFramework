#pragma once

#include "../Camera.h"

namespace pxl
{
    class OpenGLOrthographicCamera : public Camera
    {
    public:
        OpenGLOrthographicCamera();

        virtual void Update() override;

        virtual void SetFOV(float fov) override {} // orthographic cameras don't have a field of view, OOP moment
        
    private:
        virtual void RecalculateProjection() override;
    };
}