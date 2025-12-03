#pragma once

namespace pxl
{
    // values from glfw
    enum class MouseCode
    {
        // clang-format off
        Button1      = 0,
        Button2      = 1,
        Button3      = 2,
        Button4      = 3,
        Button5      = 4,
        Button6      = 5,
        Button7      = 6,
        Button8      = 7,
        LastButton   = Button8,
        LeftButton   = Button1,
        RightButton  = Button2,
        MiddleButton = Button3,
        // clang-format on
    };

    namespace Utils
    {
        inline std::string ToString(MouseCode code)
        {
            switch (code)
            {
                case MouseCode::Button1: return "Left Mouse Button";
                case MouseCode::Button2: return "Right Mouse Button";
                case MouseCode::Button3: return "Middle Mouse Button";
                case MouseCode::Button4: return "Mouse Button 4";
                case MouseCode::Button5: return "Mouse Button 5";
                case MouseCode::Button6: return "Mouse Button 6";
                case MouseCode::Button7: return "Mouse Button 7";
                case MouseCode::Button8: return "Mouse Button 8";
            }

            return "Unknown Mouse Button";
        }
    }
}