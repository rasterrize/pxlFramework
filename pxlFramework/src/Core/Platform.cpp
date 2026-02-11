#include "Platform.h"

#include <GLFW/glfw3native.h>

#ifdef _WIN64
    #include <commdlg.h>
    #include <dwmapi.h>
    #include <timeapi.h>
#endif

namespace pxl
{
    void Platform::SetMinimumTimerResolution(uint32_t value)
    {
#ifdef _WIN64
        timeBeginPeriod(value);
#endif
    }

    void Platform::ResetMinimumTimerResolution(uint32_t value)
    {
#ifdef _WIN64
        timeEndPeriod(value);
#endif
    }

    std::string Platform::OpenFile(const std::shared_ptr<Window> window, const char* filter)
    {
#ifdef _WIN64
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window(window->GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
#endif
        return std::string();
    }

    std::string Platform::SaveFile(const std::shared_ptr<Window> window, const char* filter)
    {
#ifdef _WIN64
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window(window->GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
#endif
        return std::string();
    }

    void Platform::UseImmersiveDarkMode(GLFWwindow* window)
    {
#ifdef _WIN64
        auto hwnd = glfwGetWin32Window(window);
        int useImmersiveDarkMode = 1;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useImmersiveDarkMode, sizeof(int));
#endif
    }
}