#pragma once
#include <glm/glm.hpp>

namespace pxl
{   
    enum class CameraType
    {
        Orthographic, Perspective
    };

    // struct CameraSettings
    // {

    // };

    class BaseCamera
    {
    public:
        virtual void Update() = 0;

        const glm::vec3 GetPosition() const { return m_Position; }
        const glm::vec3 GetRotation() const { return m_Rotation; }

        void SetPosition(glm::vec3 position) { m_Position = position; }
        void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; }
    protected:
        BaseCamera(CameraType cameraType, float nearClip, float farClip) : m_CameraType(cameraType), m_NearClip(nearClip), m_FarClip(farClip) {};

        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix       = glm::mat4(1.0f); // it does work with directx but it has it's own math class // probably should make own math class
        glm::mat4 m_ModelMatrix      = glm::mat4(1.0f); // still not sure if this should be here but maybe????

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f); // do all these vectors and matrices need to be initialized?

        float m_NearClip;
        float m_FarClip;

        CameraType m_CameraType;
    private:
        virtual void RecalculateProjection() = 0;
    };
}