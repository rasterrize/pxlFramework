#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Core/Application.h"

namespace pxl
{
    class OrthographicCamera;
    class PerspectiveCamera;

    struct PerspectiveSettings
    {
        float FOV = 60.0f;
        float AspectRatio = 16.0f / 9.0f;
        float NearClip = 0.001f;
        float FarClip = 1000.0f;
    };

    struct OrthographicSettings
    {
        float AspectRatio = 16.0f / 9.0f;
        float NearClip = -1.0f;
        float FarClip = 1.0f;
        float Zoom = 0.0f;
        float Left, Right, Bottom, Top;
        bool UseAspectRatio = true;
    };

    class Camera
    {
    public:
        Camera();
        virtual ~Camera();

        virtual void Update() = 0;

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec3& GetRotation() const { return m_Rotation; }

        glm::vec3 GetForwardVector();
        glm::vec3 GetUpVector();
        glm::vec3 GetRightVector();

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        /// @brief Create and return a new PerspectiveCamera
        /// @param settings Perspective camera settings
        /// @return The camera
        static std::shared_ptr<PerspectiveCamera> CreatePerspective(const PerspectiveSettings& settings);

        /// @brief Create and return a new OrthographicCamera
        /// @param settings Orthographic camera settings
        /// @return The camera
        static std::shared_ptr<OrthographicCamera> CreateOrthographic(const OrthographicSettings& settings);

    protected:
        virtual void RecalculateProjection() = 0;

    private:
        friend class Application;
        static void UpdateAll();

    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);

    private:
        static inline std::vector<Camera*> s_Cameras;
    };
}