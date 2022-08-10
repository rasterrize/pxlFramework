#include "../GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace pxl
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext();

        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHandle;
    };
}