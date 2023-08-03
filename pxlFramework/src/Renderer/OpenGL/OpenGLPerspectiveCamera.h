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
        
        virtual void Update() override;
    private:
        virtual void RecalculateProjection() override;

        float m_FOV;
    };
}