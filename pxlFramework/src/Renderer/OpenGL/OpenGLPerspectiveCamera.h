#pragma once

#include "../BaseCamera.h"

#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLPerspectiveCamera : public BaseCamera
    {
    public:
        OpenGLPerspectiveCamera();
        
        virtual void Update() override;
    private:
        virtual void RecalculateProjection() override;

        float m_FOV;
    };
}