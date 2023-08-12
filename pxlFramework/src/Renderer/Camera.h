#pragma once

#include "../Core/Application.h"

#include <glm/glm.hpp>

namespace pxl
{   
    enum class CameraType
    {
        Orthographic, Perspective
    };

    struct CameraSettings
    {
        //CameraType CameraType;
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

        void SetPosition(glm::vec3 position) { m_Position = position; }
        void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; }

        virtual void SetFOV(float fov) = 0;
        virtual float GetFOV() = 0;

        static std::shared_ptr<Camera> Create(CameraType type);
    protected:
        Camera(CameraType cameraType, const CameraSettings& cameraSettings) 
            : m_CameraSettings(cameraSettings) {};

        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix       = glm::mat4(1.0f);
        
        glm::vec3 m_Position = glm::vec3(0.0f); // not sure whether these should be per camera type yet 
        glm::vec3 m_Rotation = glm::vec3(0.0f); //

        //CameraType m_CameraType;
        CameraSettings m_CameraSettings;

        std::shared_ptr<Camera> m_Handle; // This is isn't used currently
    private:
        virtual void RecalculateProjection() = 0;

        friend class Application;
        static void UpdateAll();
    private:
        static std::vector<std::shared_ptr<Camera>> s_Cameras;
    };
}