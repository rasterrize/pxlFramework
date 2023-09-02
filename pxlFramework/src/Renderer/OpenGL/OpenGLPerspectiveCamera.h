#pragma once

#include "../Camera.h"

#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLPerspectiveCamera : public Camera
    {
    public:
        OpenGLPerspectiveCamera();

        virtual void SetFOV(float fov) override { m_FOV = fov; RecalculateProjection(); }
        virtual float GetFOV() override { return m_FOV; }
        
        virtual void Update() override;
    private:
        virtual void RecalculateProjection() override;

        float m_FOV = 45.0f;
    };
}