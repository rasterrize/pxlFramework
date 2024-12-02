#pragma once

#include "Camera.h"

namespace pxl
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(const PerspectiveSettings& settings);
        virtual ~PerspectiveCamera() = default;

        virtual void Update() override;

        void SetFOV(float fov)
        {
            m_Settings.FOV = fov;
            RecalculateProjection();
        }
        float GetFOV() const { return m_Settings.FOV; }

        void SetAspectRatio(float aspect)
        {
            m_Settings.AspectRatio = aspect;
            RecalculateProjection();
        }
        float GetAspectRatio() const { return m_Settings.AspectRatio; }

    protected:
        virtual void RecalculateProjection() override;

    private:
        PerspectiveSettings m_Settings = {};
    };
}