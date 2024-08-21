#pragma once

#include "Camera.h"

namespace pxl
{
    struct OrthographicCameraSettings
    {
        float AspectRatio;
        float NearClip;
        float FarClip;
        float Zoom;
        float Left, Right, Bottom, Top;
        bool UseAspectRatio;
    };
    
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(const OrthographicCameraSettings& settings);

        virtual void Update() override;

        void SetZoom(float zoom)
        {
            m_Settings.Zoom = zoom;
            m_Settings.UseAspectRatio ? RecalculateSidesWithAspectRatio() : RecalculateSides();
            RecalculateProjection(); 
        }
        float GetZoom() const { return m_Settings.Zoom; }

        void SetSides(float left, float right, float bottom, float top)
        {
            m_Settings.Left = left;
            m_Settings.Right = right;
            m_Settings.Bottom = bottom;
            m_Settings.Top = top;
        }

        void SetLeft(float left)     { m_Settings.Left = left; RecalculateProjection(); }
        void SetRight(float right)   { m_Settings.Right = right; RecalculateProjection(); }
        void SetBottom(float bottom) { m_Settings.Bottom = bottom; RecalculateProjection(); }
        void SetTop(float top)       { m_Settings.Top = top; RecalculateProjection(); }

        static std::shared_ptr<OrthographicCamera> Create(const OrthographicCameraSettings& settings);

    protected:
        virtual void RecalculateProjection() override;
    private:
        void RecalculateSides();
        void RecalculateSidesWithAspectRatio();
    private:
        OrthographicCameraSettings m_Settings;
    };
}