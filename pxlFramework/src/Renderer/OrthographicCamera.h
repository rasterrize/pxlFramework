#pragma once

#include "Camera.h"

namespace pxl
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(const OrthographicSettings& settings);

        RectF GetSides() const { return m_Settings.Sides; }

        void SetSides(RectF rect)
        {
            m_Settings.Sides = rect;
            RecalculateProjection();
        }

    protected:
        virtual void RecalculateProjection() override;
        virtual void RecalculateView() override;

    private:
        OrthographicSettings m_Settings = {};
    };
}