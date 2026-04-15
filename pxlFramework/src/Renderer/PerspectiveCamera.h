#pragma once

#include "Camera.h"

namespace pxl
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(const PerspectiveSettings& settings);
        virtual ~PerspectiveCamera() = default;

        void SetFOV(float fov)
        {
            m_Settings.FOV = fov;
            RecalculateProjection();
        }

        float GetFOV() const { return m_Settings.FOV; }

    protected:
        virtual void RecalculateProjection() override;
        virtual void RecalculateView() override;

    private:
        PerspectiveSettings m_Settings = {};
    };
}