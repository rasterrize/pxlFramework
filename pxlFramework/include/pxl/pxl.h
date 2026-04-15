#pragma once

// Core
#include "../src/Core/Animation.h"
#include "../src/Core/Application.h"
#include "../src/Core/Assert.h"
#include "../src/Core/Colour.h"
#include "../src/Core/Config.h"
#include "../src/Core/Cursor.h"
#include "../src/Core/Image.h"
#include "../src/Core/Input.h"
#include "../src/Core/InputSystem.h"
#include "../src/Core/KeyCodes.h"
#include "../src/Core/Logging/ApplicationLog.h"
#include "../src/Core/MouseCodes.h"
#include "../src/Core/Platform.h"
#include "../src/Core/Size.h"
#include "../src/Core/Stopwatch.h"
#include "../src/Core/Window.h"

// Events
#include "../src/Core/Events/Event.h"
#include "../src/Core/Events/EventHandler.h"
#include "../src/Core/Events/GamepadEvents.h"
#include "../src/Core/Events/KeyboardEvents.h"
#include "../src/Core/Events/MouseEvents.h"
#include "../src/Core/Events/WindowEvents.h"

// Renderer
#include "../src/Renderer/BufferLayout.h"
#include "../src/Renderer/Camera.h"
#include "../src/Renderer/GPUBuffer.h"
#include "../src/Renderer/GPUResource.h"
#include "../src/Renderer/GraphicsAPI.h"
#include "../src/Renderer/GraphicsAPIType.h"
#include "../src/Renderer/GraphicsContext.h"
#include "../src/Renderer/GraphicsPipeline.h"
#include "../src/Renderer/Mesh.h"
#include "../src/Renderer/OrthographicCamera.h"
#include "../src/Renderer/PerspectiveCamera.h"
#include "../src/Renderer/Primitives/2D/Quad.h"
#include "../src/Renderer/Primitives/2D/Sprite.h"
#include "../src/Renderer/Renderer.h"
#include "../src/Renderer/RendererConfig.h"
#include "../src/Renderer/Shader.h"
#include "../src/Renderer/ShaderCompiler.h"
#include "../src/Renderer/ShaderManager.h"
#include "../src/Renderer/Texture.h"
#include "../src/Renderer/TextureHandler.h"
#include "../src/Renderer/UniformLayout.h"
#include "../src/Renderer/VertexBatch.h"
#include "../src/Renderer/Vertices.h"

// Utils
#include "../src/Utils/Easing.h"
#include "../src/Utils/FileSystem.h"
#include "../src/Utils/Random.h"

// Debug
#include "../src/Debug/Profiler.h"

#ifdef PXL_ENABLE_MODULE_DISCORD
    #include "../modules/discord/src/DiscordRPC.h"
#endif

// Allows applications to use ImGui functions
#ifdef PXL_ENABLE_IMGUI
    #include <imgui.h>
#endif

// NOTE: For now we provide glm to the user, since it's the only math library we use.
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
