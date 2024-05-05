#pragma once

#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>

namespace pxl
{
    class TransformComponent : public Component
    {
    public:
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);

        void Translate(const glm::vec3& translation);
        void Rotate(const glm::vec3& orientation);
    private:
        glm::mat4 transform; // mat3 possibly?
    };
}
