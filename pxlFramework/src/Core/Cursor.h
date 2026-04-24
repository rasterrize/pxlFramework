#pragma once

#include <GLFW/glfw3.h>

#include "Image.h"

namespace pxl
{
    enum class CursorMode
    {
        Normal,
        Hidden,
        Disabled,
        Captured,
    };

    enum class StandardCursor
    {
        Arrow,
        IBeam,
        Crosshair,
        Hand,
        HResize,
        VResize,
    };

    class Cursor
    {
    public:
        Cursor(const std::shared_ptr<Image>& image, int32_t hotspotX = 0, int32_t hotspotY = 0)
        {
            GLFWimage glfwImage;
            glfwImage.width = image->Metadata.Size.Width;
            glfwImage.height = image->Metadata.Size.Height;
            glfwImage.pixels = image->Buffer.data();

            m_Cursor = glfwCreateCursor(&glfwImage, hotspotX, hotspotY);
        }

        ~Cursor()
        {
            glfwDestroyCursor(m_Cursor);
        }

        GLFWcursor* GetNativeCursor() const { return m_Cursor; }

    private:
        GLFWcursor* m_Cursor = nullptr;
    };

    namespace Utils
    {
        inline std::string ToString(CursorMode mode)
        {
            switch (mode)
            {
                case CursorMode::Normal:   return "Normal";
                case CursorMode::Hidden:   return "Hidden";
                case CursorMode::Disabled: return "Disabled";
                case CursorMode::Captured: return "Captured";
                default:                   return "Unknown";
            }
        }
    }
}