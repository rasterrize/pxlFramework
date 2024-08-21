#pragma once

#include "Camera.h"

namespace pxl
{
    struct PerspectiveCameraSettings
    {
        float FOV;
        float AspectRatio;
        float NearClip;
        float FarClip;
    };
    
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(const PerspectiveCameraSettings& settings);

        void SetFOV(float fov) { m_Settings.FOV = fov; RecalculateProjection(); }
        float GetFOV() const { return m_Settings.FOV; }
        
        virtual void Update() override;

        static std::shared_ptr<PerspectiveCamera> Create(const PerspectiveCameraSettings& settings);
    protected:
        virtual void RecalculateProjection() override;
    private:
        PerspectiveCameraSettings m_Settings;
    };
}