#pragma once

namespace pxl
{
    // NOTE: values from glfw
    enum class GamepadButton
    {
        // clang-format off
        A           = 0,
        B           = 1,
        X           = 2,
        Y           = 3,
        LeftBumper  = 4,
        RightBumper = 5,
        Back        = 6,
        Start       = 7,
        Guide       = 8,
        LeftThumb   = 9,
        RightThumb  = 10,
        DPadUp      = 11,
        DPadRight   = 12,
        DPadDown    = 13,
        DPadLeft    = 14,

        Cross    = A,
        Circle   = B,
        Square   = X,
        Triangle = Y,
        // clang-format on
    };

    enum class GamepadAxis
    {
        // clang-format off
        LeftThumbX   = 0,
        LeftThumbY   = 1,
        RightThumbX  = 2,
        RightThumbY  = 3,
        LeftTrigger  = 4,
        RightTrigger = 5,
        // clang-format on
    };

    namespace Utils
    {
        inline std::string ToString(GamepadButton button)
        {
            switch (button)
            {
                case GamepadButton::A:           return "A";
                case GamepadButton::B:           return "B";
                case GamepadButton::X:           return "X";
                case GamepadButton::Y:           return "Y";
                case GamepadButton::LeftBumper:  return "LeftBumper";
                case GamepadButton::RightBumper: return "RightBumper";
                case GamepadButton::Back:        return "Back";
                case GamepadButton::Start:       return "Start";
                case GamepadButton::Guide:       return "Guide";
                case GamepadButton::LeftThumb:   return "LeftThumb";
                case GamepadButton::RightThumb:  return "RightThumb";
                case GamepadButton::DPadUp:      return "DPadUp";
                case GamepadButton::DPadRight:   return "DPadRight";
                case GamepadButton::DPadDown:    return "DPadDown";
                case GamepadButton::DPadLeft:    return "DPadLeft";
            }

            return "Unknown Button";
        }

        inline std::string ToString(GamepadAxis axis)
        {
            switch (axis)
            {
                case GamepadAxis::LeftThumbX:   return "LeftThumbX";
                case GamepadAxis::LeftThumbY:   return "LeftThumbY";
                case GamepadAxis::RightThumbX:  return "RightThumbX";
                case GamepadAxis::RightThumbY:  return "RightThumbY";
                case GamepadAxis::LeftTrigger:  return "LeftTrigger";
                case GamepadAxis::RightTrigger: return "RightTrigger";
            }

            return "Unknown Axis";
        }
    }
}