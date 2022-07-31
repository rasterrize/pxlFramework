#pragma once

namespace pxl
{
    // from glfw
    enum class KeyCode
    {
        PXL_KEY_SPACE             = 32,
        PXL_KEY_APOSTROPHE        = 39,  /* ' */
        PXL_KEY_COMMA             = 44,  /* , */
        PXL_KEY_MINUS             = 45,  /* - */
        PXL_KEY_PERIOD            = 46,  /* . */
        PXL_KEY_SLASH             = 47,  /* / */
        PXL_KEY_0                 = 48,
        PXL_KEY_1                 = 49,
        PXL_KEY_2                 = 50,
        PXL_KEY_3                 = 51,
        PXL_KEY_4                 = 52,
        PXL_KEY_5                 = 53,
        PXL_KEY_6                 = 54,
        PXL_KEY_7                 = 55,
        PXL_KEY_8                 = 56,
        PXL_KEY_9                 = 57,
        PXL_KEY_SEMICOLON         = 59,  /* ; */
        PXL_KEY_EQUAL             = 61,  /* = */
        PXL_KEY_A                 = 65,
        PXL_KEY_B                 = 66,
        PXL_KEY_C                 = 67,
        PXL_KEY_D                 = 68,
        PXL_KEY_E                 = 69,
        PXL_KEY_F                 = 70,
        PXL_KEY_G                 = 71,
        PXL_KEY_H                 = 72,
        PXL_KEY_I                 = 73,
        PXL_KEY_J                 = 74,
        PXL_KEY_K                 = 75,
        PXL_KEY_L                 = 76,
        PXL_KEY_M                 = 77,
        PXL_KEY_N                 = 78,
        PXL_KEY_O                 = 79,
        PXL_KEY_P                 = 80,
        PXL_KEY_Q                 = 81,
        PXL_KEY_R                 = 82,
        PXL_KEY_S                 = 83,
        PXL_KEY_T                 = 84,
        PXL_KEY_U                 = 85,
        PXL_KEY_V                 = 86,
        PXL_KEY_W                 = 87,
        PXL_KEY_X                 = 88,
        PXL_KEY_Y                 = 89,
        PXL_KEY_Z                 = 90,
        PXL_KEY_LEFT_BRACKET      = 91,  /* [ */
        PXL_KEY_BACKSLASH         = 92,  /* \ */
        PXL_KEY_RIGHT_BRACKET     = 93,  /* ] */
        PXL_KEY_GRAVE_ACCENT      = 96,  /* ` */
        PXL_KEY_WORLD_1           = 161, /* non-US #1 */
        PXL_KEY_WORLD_2           = 162, /* non-US #2 */

        /* Function keys */
        PXL_KEY_ESCAPE            = 256,
        PXL_KEY_ENTER             = 257,
        PXL_KEY_TAB               = 258,
        PXL_KEY_BACKSPACE         = 259,
        PXL_KEY_INSERT            = 260,
        PXL_KEY_DELETE            = 261,
        PXL_KEY_RIGHT             = 262,
        PXL_KEY_LEFT              = 263,
        PXL_KEY_DOWN              = 264,
        PXL_KEY_UP                = 265,
        PXL_KEY_PAGE_UP           = 266,
        PXL_KEY_PAGE_DOWN         = 267,
        PXL_KEY_HOME              = 268,
        PXL_KEY_END               = 269,
        PXL_KEY_CAPS_LOCK         = 280,
        PXL_KEY_SCROLL_LOCK       = 281,
        PXL_KEY_NUM_LOCK          = 282,
        PXL_KEY_PRINT_SCREEN      = 283,
        PXL_KEY_PAUSE             = 284,
        PXL_KEY_F1                = 290,
        PXL_KEY_F2                = 291,
        PXL_KEY_F3                = 292,
        PXL_KEY_F4                = 293,
        PXL_KEY_F5                = 294,
        PXL_KEY_F6                = 295,
        PXL_KEY_F7                = 296,
        PXL_KEY_F8                = 297,
        PXL_KEY_F9                = 298,
        PXL_KEY_F10               = 299,
        PXL_KEY_F11               = 300,
        PXL_KEY_F12               = 301,
        PXL_KEY_F13               = 302,
        PXL_KEY_F14               = 303,
        PXL_KEY_F15               = 304,
        PXL_KEY_F16               = 305,
        PXL_KEY_F17               = 306,
        PXL_KEY_F18               = 307,
        PXL_KEY_F19               = 308,
        PXL_KEY_F20               = 309,
        PXL_KEY_F21               = 310,
        PXL_KEY_F22               = 311,
        PXL_KEY_F23               = 312,
        PXL_KEY_F24               = 313,
        PXL_KEY_F25               = 314,
        PXL_KEY_KP_0              = 320,
        PXL_KEY_KP_1              = 321,
        PXL_KEY_KP_2              = 322,
        PXL_KEY_KP_3              = 323,
        PXL_KEY_KP_4              = 324,
        PXL_KEY_KP_5              = 325,
        PXL_KEY_KP_6              = 326,
        PXL_KEY_KP_7              = 327,
        PXL_KEY_KP_8              = 328,
        PXL_KEY_KP_9              = 329,
        PXL_KEY_KP_DECIMAL        = 330,
        PXL_KEY_KP_DIVIDE         = 331,
        PXL_KEY_KP_MULTIPLY       = 332,
        PXL_KEY_KP_SUBTRACT       = 333,
        PXL_KEY_KP_ADD            = 334,
        PXL_KEY_KP_ENTER          = 335,
        PXL_KEY_KP_EQUAL          = 336,
        PXL_KEY_LEFT_SHIFT        = 340,
        PXL_KEY_LEFT_CONTROL      = 341,
        PXL_KEY_LEFT_ALT          = 342,
        PXL_KEY_LEFT_SUPER        = 343,
        PXL_KEY_RIGHT_SHIFT       = 344,
        PXL_KEY_RIGHT_CONTROL     = 345,
        PXL_KEY_RIGHT_ALT         = 346,
        PXL_KEY_RIGHT_SUPER       = 347,
        PXL_KEY_MENU              = 348,
    };
}