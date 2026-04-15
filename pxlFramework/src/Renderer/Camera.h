#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Core/Rect.h"

namespace pxl
{
    struct CameraSettings
    {
        float NearClip = 0.001f;
        float FarClip = 1000.0f;
        float AspectRatio = 16.0f / 9.0f;
    };

    struct PerspectiveSettings
    {
        CameraSettings CameraSettings = {};
        float FOV = 60.0f;
    };

    struct OrthographicSettings
    {
        CameraSettings CameraSettings = {};
        float Zoom = 1.0f;
        RectF Sides = { -1.0f, 1.0f, -1.0f, 1.0f };
    };

    class Camera
    {
    public:
        virtual ~Camera() = default;

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec3& GetRotation() const { return m_Rotation; }

        void SetPosition(const glm::vec3& position)
        {
            m_Position = position;
            RecalculateView();
        }
        void SetRotation(const glm::vec3& rotation)
        {
            m_Rotation = rotation;
            RecalculateView();
        }

        void Translate(const glm::vec3& translation)
        {
            m_Position += translation;
            RecalculateView();
        }

        void Translate(float translation, const glm::vec3& vector)
        {
            m_Position += vector * translation;
            RecalculateView();
        }

        void Rotate(const glm::vec3& rotation)
        {
            m_Rotation += rotation;
            RecalculateView();
        }

        glm::vec3 GetForwardVector();
        glm::vec3 GetUpVector();
        glm::vec3 GetRightVector();

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

    protected:
        virtual void RecalculateProjection() = 0;
        virtual void RecalculateView() = 0;

    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);
    };
}