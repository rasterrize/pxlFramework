#pragma once

#include "../Camera.h"

namespace pxl
{
    class OpenGLPerspectiveCamera : public Camera
    {
    public:
        OpenGLPerspectiveCamera();

        virtual void SetFOV(float fov) override { m_FOV = fov; RecalculateProjection(); }
        virtual float GetFOV() override { return m_FOV; }

        virtual void SetZoom(float zoom) override {}
        virtual float GetZoom() override { return 0.0f; }
        
        virtual void Update() override;
    private:
        virtual void RecalculateProjection() override;

        float m_FOV = 45.0f;
    };
}