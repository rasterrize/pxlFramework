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
        virtual float GetFOV() override { return 0.0f; }

        virtual void SetZoom(float zoom) override { m_Zoom = zoom; RecalculateSides(); RecalculateProjection(); }
        virtual float GetZoom() override { return m_Zoom; }
        
    private:
        virtual void RecalculateProjection() override;

        void RecalculateSides();
    private:
        float m_Left, m_Right, m_Bottom, m_Top;

        float m_Zoom = 1.0f;
    };
}