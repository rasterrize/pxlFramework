#pragma once

#include "../Core/Application.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace pxl
{   
    enum class CameraType
    {
        Orthographic, Perspective
    };

    struct CameraSettings
    {
        float AspectRatio;
        float NearClip;
        float FarClip;
    };

    class Camera
    {
    public:
        virtual void Update() = 0;

        const glm::vec3 GetPosition() const { return m_Position; }
        const glm::vec3 GetRotation() const { return m_Rotation; }

        const glm::vec3 GetForwardVector();
        const glm::vec3 GetUpVector();
        const glm::vec3 GetRightVector();

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

        virtual void SetFOV(float fov) = 0;
        virtual float GetFOV() = 0;

        virtual void SetZoom(float zoom) = 0;
        virtual float GetZoom() = 0;

        const glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        static std::shared_ptr<Camera> Create(CameraType type, const CameraSettings& settings);
    protected:
        Camera(CameraType type, const CameraSettings& cameraSettings) 
            : m_CameraType(type), m_CameraSettings(cameraSettings) {};

        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix       = glm::mat4(1.0f);
        
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);

        CameraType m_CameraType;
        CameraSettings m_CameraSettings;

        std::shared_ptr<Camera> m_Handle;
    private:
        virtual void RecalculateProjection() = 0;

        friend class Application;
        static void UpdateAll();
    private:
        static std::vector<std::shared_ptr<Camera>> s_Cameras;
    };
}