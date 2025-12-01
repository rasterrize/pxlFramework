#include "Platform.h"

#if (_WIN32)
    #include <GLFW/glfw3native.h>
    #include <commdlg.h>
    #include <timeapi.h>
#endif

namespace pxl
{
    void Platform::SetMinimumTimerResolution(uint32_t value)
    {
#if (_WIN32)
        timeBeginPeriod(value);
#endif
    }

    void Platform::ResetMinimumTimerResolution(uint32_t value)
    {
#if (_WIN32)
        timeEndPeriod(value);
#endif
    }

    std::string pxl::Platform::OpenFile(const std::shared_ptr<Window> window, const char* filter)
    {
#if (_WIN32)
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

    std::string pxl::Platform::SaveFile(const std::shared_ptr<Window> window, const char* filter)
    {
#if (_WIN32)
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
}