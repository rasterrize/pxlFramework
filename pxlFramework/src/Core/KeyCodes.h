#pragma once

namespace pxl
{
    // NOTE: values from glfw
    enum class KeyCode
    {
        // clang-format off
        Space             = 32,
        Apostrophe        = 39,  /* ' */
        Comma             = 44,  /* , */
        Minus             = 45,  /* - */
        Period            = 46,  /* . */
        Slash             = 47,  /* / */
        Num1              = 49,
        Num0              = 48,
        Num2              = 50,
        Num3              = 51,
        Num4              = 52,
        Num5              = 53,
        Num6              = 54,
        Num7              = 55,
        Num8              = 56,
        Num9              = 57,
        Semicolon         = 59,  /* ; */
        Equal             = 61,  /* = */
        A                 = 65,
        B                 = 66,
        C                 = 67,
        D                 = 68,
        E                 = 69,
        F                 = 70,
        G                 = 71,
        H                 = 72,
        I                 = 73,
        J                 = 74,
        K                 = 75,
        L                 = 76,
        M                 = 77,
        N                 = 78,
        O                 = 79,
        P                 = 80,
        Q                 = 81,
        R                 = 82,
        S                 = 83,
        T                 = 84,
        U                 = 85,
        V                 = 86,
        W                 = 87,
        X                 = 88,
        Y                 = 89,
        Z                 = 90,
        LeftBracket       = 91,  /* [ */
        Backslash         = 92,  /* \ */
        RightBracket      = 93,  /* ] */
        GraveAccent       = 96,  /* ` */
        World1            = 161, /* non-US #1 */
        World2            = 162, /* non-US #2 */

        /* Function keys */
        Escape            = 256,
        Enter             = 257,
        Tab               = 258,
        Backspace         = 259,
        Insert            = 260,
        Delete            = 261,
        Right             = 262,
        Left              = 263,
        Down              = 264,
        Up                = 265,
        PageUp            = 266,
        PageDown          = 267,
        Home              = 268,
        End               = 269,
        CapsLock          = 280,
        ScrollLock        = 281,
        NumLock           = 282,
        PrintScreen       = 283,
        Pause             = 284,
        F1                = 290,
        F2                = 291,
        F3                = 292,
        F4                = 293,
        F5                = 294,
        F6                = 295,
        F7                = 296,
        F8                = 297,
        F9                = 298,
        F10               = 299,
        F11               = 300,
        F12               = 301,
        F13               = 302,
        F14               = 303,
        F15               = 304,
        F16               = 305,
        F17               = 306,
        F18               = 307,
        F19               = 308,
        F20               = 309,
        F21               = 310,
        F22               = 311,
        F23               = 312,
        F24               = 313,
        F25               = 314,
        NumPad0           = 320,
        NumPad1           = 321,
        NumPad2           = 322,
        NumPad3           = 323,
        NumPad4           = 324,
        NumPad5           = 325,
        NumPad6           = 326,
        NumPad7           = 327,
        NumPad8           = 328,
        NumPad9           = 329,
        NumPadDecimal     = 330,
        NumPadDivide      = 331,
        NumPadMultiply    = 332,
        NumPadSubtract    = 333,
        NumPadAdd         = 334,
        NumPadEnter       = 335,
        NumPadEqual       = 336,
        LeftShift         = 340,
        LeftControl       = 341,
        LeftAlt           = 342,
        LeftSuper         = 343,
        RightShift        = 344,
        RightControl      = 345,
        RightAlt          = 346,
        RightSuper        = 347,
        Menu              = 348,
        // clang-format on
    };

    // NOTE: values from glfw
    enum KeyModFlags
    {
        // clang-format off
        Shift    = 0x0001,
        Control  = 0x002,
        Alt      = 0x004,
        Super    = 0x0008,
        CapsLock = 0x010,
        NumLock  = 0x0020,
        // clang-format on
    };

    namespace Utils
    {
        inline std::string ToString(KeyCode code)
        {
            switch (code)
            {
                case KeyCode::Space:          return "Space";
                case KeyCode::Apostrophe:     return "'";
                case KeyCode::Comma:          return ",";
                case KeyCode::Minus:          return "-";
                case KeyCode::Period:         return "=";
                case KeyCode::Slash:          return "/";
                case KeyCode::Num1:           return "1";
                case KeyCode::Num0:           return "0";
                case KeyCode::Num2:           return "2";
                case KeyCode::Num3:           return "3";
                case KeyCode::Num4:           return "4";
                case KeyCode::Num5:           return "5";
                case KeyCode::Num6:           return "6";
                case KeyCode::Num7:           return "7";
                case KeyCode::Num8:           return "8";
                case KeyCode::Num9:           return "9";
                case KeyCode::Semicolon:      return ";";
                case KeyCode::Equal:          return "=";
                case KeyCode::A:              return "A";
                case KeyCode::B:              return "B";
                case KeyCode::C:              return "C";
                case KeyCode::D:              return "D";
                case KeyCode::E:              return "E";
                case KeyCode::F:              return "F";
                case KeyCode::G:              return "G";
                case KeyCode::H:              return "H";
                case KeyCode::I:              return "I";
                case KeyCode::J:              return "J";
                case KeyCode::K:              return "K";
                case KeyCode::L:              return "L";
                case KeyCode::M:              return "M";
                case KeyCode::N:              return "N";
                case KeyCode::O:              return "O";
                case KeyCode::P:              return "P";
                case KeyCode::Q:              return "Q";
                case KeyCode::R:              return "R";
                case KeyCode::S:              return "S";
                case KeyCode::T:              return "T";
                case KeyCode::U:              return "U";
                case KeyCode::V:              return "V";
                case KeyCode::W:              return "W";
                case KeyCode::X:              return "X";
                case KeyCode::Y:              return "Y";
                case KeyCode::Z:              return "Z";
                case KeyCode::LeftBracket:    return "[";
                case KeyCode::Backslash:      return "Backslash";
                case KeyCode::RightBracket:   return "]";
                case KeyCode::GraveAccent:    return "`";
                case KeyCode::World1:         return "World 1";
                case KeyCode::World2:         return "World 2";
                case KeyCode::Escape:         return "Escape";
                case KeyCode::Enter:          return "Enter";
                case KeyCode::Tab:            return "Tab";
                case KeyCode::Backspace:      return "Backspace";
                case KeyCode::Insert:         return "Insert";
                case KeyCode::Delete:         return "Delete";
                case KeyCode::Right:          return "Right Arrow";
                case KeyCode::Left:           return "Left Arrow";
                case KeyCode::Down:           return "Down Arrow";
                case KeyCode::Up:             return "Up Arrow";
                case KeyCode::PageUp:         return "Page Up";
                case KeyCode::PageDown:       return "Page Down";
                case KeyCode::Home:           return "Home";
                case KeyCode::End:            return "End";
                case KeyCode::CapsLock:       return "Caps Lock";
                case KeyCode::ScrollLock:     return "Scroll Lock";
                case KeyCode::NumLock:        return "Num Lock";
                case KeyCode::PrintScreen:    return "Print Screen";
                case KeyCode::Pause:          return "Pause";
                case KeyCode::F1:             return "F1";
                case KeyCode::F2:             return "F2";
                case KeyCode::F3:             return "F3";
                case KeyCode::F4:             return "F4";
                case KeyCode::F5:             return "F5";
                case KeyCode::F6:             return "F6";
                case KeyCode::F7:             return "F7";
                case KeyCode::F8:             return "F8";
                case KeyCode::F9:             return "F9";
                case KeyCode::F10:            return "F10";
                case KeyCode::F11:            return "F11";
                case KeyCode::F12:            return "F12";
                case KeyCode::F13:            return "F13";
                case KeyCode::F14:            return "F14";
                case KeyCode::F15:            return "F15";
                case KeyCode::F16:            return "F16";
                case KeyCode::F17:            return "F17";
                case KeyCode::F18:            return "F18";
                case KeyCode::F19:            return "F19";
                case KeyCode::F20:            return "F20";
                case KeyCode::F21:            return "F21";
                case KeyCode::F22:            return "F22";
                case KeyCode::F23:            return "F23";
                case KeyCode::F24:            return "F24";
                case KeyCode::F25:            return "F25";
                case KeyCode::NumPad0:        return "NumPad 0";
                case KeyCode::NumPad1:        return "NumPad 1";
                case KeyCode::NumPad2:        return "NumPad 2";
                case KeyCode::NumPad3:        return "NumPad 3";
                case KeyCode::NumPad4:        return "NumPad 4";
                case KeyCode::NumPad5:        return "NumPad 5";
                case KeyCode::NumPad6:        return "NumPad 6";
                case KeyCode::NumPad7:        return "NumPad 7";
                case KeyCode::NumPad8:        return "NumPad 8";
                case KeyCode::NumPad9:        return "NumPad 9";
                case KeyCode::NumPadDecimal:  return "NumPad Decimal";
                case KeyCode::NumPadDivide:   return "NumPad Divide";
                case KeyCode::NumPadMultiply: return "NumPad Multiply";
                case KeyCode::NumPadSubtract: return "NumPad Subtract";
                case KeyCode::NumPadAdd:      return "NumPad Add";
                case KeyCode::NumPadEnter:    return "NumPad Enter";
                case KeyCode::NumPadEqual:    return "NumPad Equal";
                case KeyCode::LeftShift:      return "Left Shift";
                case KeyCode::LeftControl:    return "Left Control";
                case KeyCode::LeftAlt:        return "Left Alt";
                case KeyCode::LeftSuper:      return "Left Super";
                case KeyCode::RightShift:     return "Right Shift";
                case KeyCode::RightControl:   return "Right Control";
                case KeyCode::RightAlt:       return "Right Alt";
                case KeyCode::RightSuper:     return "Right Super";
                case KeyCode::Menu:           return "Menu";
            }

            return "Unknown Key";
        }
    }
}