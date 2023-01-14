#include "../BaseCamera.h"
#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLPerspectiveCamera : public BaseCamera
    {
    public:
        OpenGLPerspectiveCamera();
        
        virtual void Update() override;
    private:

        //glm::mat4 ProjectionMatrix;glm::mat4 m_ProjectionMatrix;
        //glm::mat4 m_ViewMatrix;
        glm::mat4 m_ModelMatrix; // not sure bout this

        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
    };
}