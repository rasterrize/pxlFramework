#pragma once

#include "../src/Core/Application.h"
#include "../src/Core/Window.h"
#include "../src/Core/Logging/ApplicationLog.h"
#include "../src/Core/Input.h"
#include "../src/Core/KeyCodes.h"
#include "../src/Core/MouseCodes.h"
#include "../src/Core/Platform.h"
#include "../src/Core/Stopwatch.h"
#include "../src/Core/Config.h"
#include "../src/Core/DiscordRPC.h"

#include "../src/Renderer/Renderer.h"
#include "../src/Renderer/Camera.h"
#include "../src/Renderer/Shader.h"
#include "../src/Renderer/ShaderLibrary.h"
#include "../src/Renderer/Pipeline.h"
#include "../src/Renderer/BufferLayout.h"

//#include "../src/Audio/AudioManager.h"

#include "../src/Debug/ImGui/pxl_ImGui.h"
#include <imgui.h> // so applications can use ImGui functions

#include "../src/Utils/FileLoader.h"
#include "../src/Utils/EnumStringHelper.h"