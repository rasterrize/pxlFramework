#pragma once
#include <glm/glm.hpp>

namespace pxl
{   
    class BaseCamera
    {
    public:
        virtual void Update() = 0;

        const glm::vec3 GetPosition() const { return m_Position; }
        void SetPosition(glm::vec3 position) { m_Position = position; }
        // template<typename T>
        // void SetPosition(T value);
    protected:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f); // uses opengl math which might not work with directx

        glm::vec3 m_Position = glm::vec3(0.0f);
    private:
        //virtual void SetRotation() = 0; // may not work (params might not match up between classes)
    };
}