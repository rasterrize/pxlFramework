#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Core/Application.h"

namespace pxl
{
    // enum class ProjectionType
    // {
    //     None,
    //     Orthographic,
    //     Perspective,
    // };

    class Camera
    {
    public:
        virtual ~Camera() = default;

        virtual void Update() = 0;

        const glm::vec3 GetPosition() const { return m_Position; }
        const glm::vec3 GetRotation() const { return m_Rotation; }

        const glm::vec3 GetForwardVector();
        const glm::vec3 GetUpVector();
        const glm::vec3 GetRightVector();

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
    protected:
        virtual void RecalculateProjection() = 0;

        static void Add(const std::shared_ptr<Camera>& camera) { s_Cameras.push_back(camera); }
    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);

        std::weak_ptr<Camera> m_Handle;
    private:
        friend class Application;
        static void UpdateAll();
    private:
        static std::vector<std::shared_ptr<Camera>> s_Cameras;
    };
}