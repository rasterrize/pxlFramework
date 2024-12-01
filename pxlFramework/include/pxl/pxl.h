#pragma once

// Core
#include "../src/Core/Application.h"
#include "../src/Core/Assert.h"
#include "../src/Core/Colour.h"
#include "../src/Core/Config.h"
#include "../src/Core/Image.h"
#include "../src/Core/Input.h"
#include "../src/Core/KeyCodes.h"
#include "../src/Core/Logging/ApplicationLog.h"
#include "../src/Core/MouseCodes.h"
#include "../src/Core/Platform.h"
#include "../src/Core/Stopwatch.h"
#include "../src/Core/Window.h"

// Renderer
#include "../src/Renderer/BufferLayout.h"
#include "../src/Renderer/Camera.h"
#include "../src/Renderer/GraphicsContext.h"
#include "../src/Renderer/OrthographicCamera.h"
#include "../src/Renderer/PerspectiveCamera.h"
#include "../src/Renderer/Pipeline.h"
#include "../src/Renderer/Primitives/Quad.h"
#include "../src/Renderer/Renderer.h"
#include "../src/Renderer/RendererAPIType.h"
#include "../src/Renderer/RendererData.h"
#include "../src/Renderer/Shader.h"
#include "../src/Renderer/ShaderManager.h"
#include "../src/Renderer/Texture.h"
#include "../src/Renderer/UniformLayout.h"
#include "../src/Renderer/Vertices.h"

// Audio
//#include "../src/Audio/AudioManager.h"

// Debug
#include <imgui.h> // so applications can use ImGui functions

#include "../src/Debug/GUI/GUI.h"
#include "../src/Debug/Profiler.h"
#include "../src/Utils/DiscordRPC.h"
#include "../src/Utils/EnumStringHelper.h"
#include "../src/Utils/FileSystem.h"