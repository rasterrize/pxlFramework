cmake_minimum_required(VERSION 3.5.0)
project(pxlFramework VERSION 0.0.1 LANGUAGES CXX)

add_library(pxlFramework STATIC
    # PCH
    src/PCH.h

    # CORE
    src/Core/Application.h src/Core/Application.cpp
    src/Core/Window.h src/Core/Window.cpp
    src/Core/Logging/Logger.h src/Core/Logging/Logger.cpp
    src/Core/Logging/FrameworkLog.h
    src/Core/Logging/ApplicationLog.h
    src/Core/Input.h src/Core/Input.cpp
    src/Core/KeyCodes.h
    src/Core/MouseCodes.h
    src/Core/Platform.h src/Core/Platform.cpp
    src/Core/Stopwatch.h
    src/Core/Config.h src/Core/Config.cpp
    src/Core/Size.h
    src/Core/Colour.h
    src/Core/Image.h

    # RENDERER
    src/Renderer/Renderer.h src/Renderer/Renderer.cpp
    src/Renderer/RendererAPI.h src/Renderer/RendererAPI.cpp
    src/Renderer/RendererAPIType.h
    src/Renderer/GraphicsContext.h src/Renderer/GraphicsContext.cpp
    src/Renderer/GPUBuffer.h src/Renderer/GPUBuffer.cpp
    src/Renderer/VertexArray.h src/Renderer/VertexArray.cpp
    src/Renderer/BufferLayout.h
    src/Renderer/UniformLayout.h
    src/Renderer/Shader.h src/Renderer/Shader.cpp
    src/Renderer/Camera.h src/Renderer/Camera.cpp
    src/Renderer/OrthographicCamera.h src/Renderer/OrthographicCamera.cpp
    src/Renderer/PerspectiveCamera.h src/Renderer/PerspectiveCamera.cpp
    src/Renderer/Texture.h src/Renderer/Texture.cpp
    src/Renderer/ShaderLibrary.h src/Renderer/ShaderLibrary.cpp
    src/Renderer/RendererData.h
    src/Renderer/Pipeline.h src/Renderer/Pipeline.cpp
    src/Renderer/Framebuffer.h
    src/Renderer/GraphicsDevice.h
    src/Renderer/Vertices.h

    # PRIMITIVES
    src/Renderer/Primitives/Primitive.h
    src/Renderer/Primitives/Quad.h
    src/Renderer/Primitives/Cube.h
    src/Renderer/Primitives/Line.h

    # OPENGL
    src/Renderer/OpenGL/OpenGLRenderer.h src/Renderer/OpenGL/OpenGLRenderer.cpp
    src/Renderer/OpenGL/OpenGLContext.h src/Renderer/OpenGL/OpenGLContext.cpp
    src/Renderer/OpenGL/OpenGLBuffer.h src/Renderer/OpenGL/OpenGLBuffer.cpp
    src/Renderer/OpenGL/OpenGLVertexArray.h src/Renderer/OpenGL/OpenGLVertexArray.cpp
    src/Renderer/OpenGL/OpenGLShader.h src/Renderer/OpenGL/OpenGLShader.cpp
    src/Renderer/OpenGL/OpenGLTexture.h src/Renderer/OpenGL/OpenGLTexture.cpp
    src/Renderer/OpenGL/OpenGLPipeline.h src/Renderer/OpenGL/OpenGLPipeline.cpp

    # AUDIO
    # src/Audio/AudioManager.h src/Audio/AudioManager.cpp
    # src/Audio/AudioTrack.h src/Audio/AudioTrack.cpp

    # VULKAN
    src/Renderer/Vulkan/VulkanInstance.h src/Renderer/Vulkan/VulkanInstance.cpp
    src/Renderer/Vulkan/VulkanRenderer.h src/Renderer/Vulkan/VulkanRenderer.cpp
    src/Renderer/Vulkan/VulkanContext.h src/Renderer/Vulkan/VulkanContext.cpp
    src/Renderer/Vulkan/VulkanShader.h src/Renderer/Vulkan/VulkanShader.cpp
    src/Renderer/Vulkan/VulkanPipeline.h src/Renderer/Vulkan/VulkanPipeline.cpp
    src/Renderer/Vulkan/VulkanRenderPass.h src/Renderer/Vulkan/VulkanRenderPass.cpp
    src/Renderer/Vulkan/VulkanFramebuffer.h src/Renderer/Vulkan/VulkanFramebuffer.cpp
    src/Renderer/Vulkan/VulkanSwapchain.h src/Renderer/Vulkan/VulkanSwapchain.cpp
    src/Renderer/Vulkan/VulkanHelpers.h src/Renderer/Vulkan/VulkanHelpers.cpp
    src/Renderer/Vulkan/VulkanImage.h src/Renderer/Vulkan/VulkanImage.cpp
    src/Renderer/Vulkan/VulkanBuffer.h src/Renderer/Vulkan/VulkanBuffer.cpp
    src/Renderer/Vulkan/VulkanDevice.h src/Renderer/Vulkan/VulkanDevice.cpp
    src/Renderer/Vulkan/VulkanAllocator.h src/Renderer/Vulkan/VulkanAllocator.cpp
    src/Renderer/Vulkan/VmaUsage.cpp

    # UTILS
    src/Utils/FileSystem.h src/Utils/FileSystem.cpp
    src/Utils/EnumStringHelper.h src/Utils/EnumStringHelper.cpp
    src/Utils/DiscordRPC.h src/Utils/DiscordRPC.cpp

    # DEBUG
    src/Debug/GUI/GUI.h src/Debug/GUI/GUI.cpp
    src/Debug/GUI/GUIBase.h
    src/Debug/GUI/GUIOpenGL.h src/Debug/GUI/GUIOpenGL.cpp
    src/Debug/GUI/GUIVulkan.h src/Debug/GUI/GUIVulkan.cpp
    src/Debug/Profiler.h src/Debug/Profiler.cpp
)

# Set project c++ standard
target_compile_features(pxlFramework PRIVATE cxx_std_20)

# Enable logging if desired
if (PXL_ENABLE_LOGGING)
    target_compile_definitions(pxlFramework PUBLIC PXL_ENABLE_LOGGING)
endif()

# Enable asserts if desired
if (PXL_ENABLE_ASSERTS)
    target_compile_definitions(pxlFramework PUBLIC PXL_ENABLE_ASSERTS)
endif()

# Enable profiling if desired
if (PXL_ENABLE_PROFILING)
    option(TRACY_ENABLE "" ON)
    option(TRACY_ON_DEMAND "" ON)
    add_subdirectory(deps/tracy)
    target_compile_definitions(pxlFramework PRIVATE PXL_ENABLE_PROFILING)
    target_link_libraries(pxlFramework Tracy::TracyClient)
    message("pxlFramework profiling enabled")
else()
    option(TRACY_ENABLE "" OFF)
endif()

# Set MSVC settings
if (MSVC)
    # Set MSVC runtime library based on build type
    set_target_properties(pxlFramework PROPERTIES CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    
    # Set MSBuild multi-processor compilation
    target_compile_options(pxlFramework PRIVATE "/MP")

    # Set compiler warning level
    target_compile_options(pxlFramework PRIVATE "/W4")

    # Treat warnings as errors
    target_compile_options(pxlFramework PRIVATE "/WX")

    # Ignore missing PDB warnings (currently because of shaderc)
    target_link_options(pxlFramework PUBLIC "/ignore:4099")
endif()

# Set GLFW settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set GLM settings
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)

# Set ASSIMP settings
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

# Set YAMLCPP settings
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)

# Add Dependencies (CMake projects)
add_subdirectory(deps/GLFW)
add_subdirectory(deps/Glad)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/GLM)
add_subdirectory(deps/ImGui)
add_subdirectory(deps/stb)
add_subdirectory(deps/assimp)
add_subdirectory(deps/yaml-cpp)
add_subdirectory(deps/discord-game-sdk)

# BASS audio library
target_include_directories(pxlFramework PUBLIC deps/BASS/include)
target_link_directories(pxlFramework PUBLIC deps/BASS/lib/x64)

# Set libraries to build with multi-processor compilation
if (MSVC)
    target_compile_options(glfw PUBLIC "/MP")
    target_compile_options(glad PUBLIC "/MP")
    target_compile_options(discordgamesdk PUBLIC "/MP")
    target_compile_options(imgui PUBLIC "/MP")
    target_compile_options(stb PUBLIC "/MP")
    target_compile_options(yaml-cpp PUBLIC "/MP")
endif()

# Find Vulkan SDK
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

# Add Vulkan include directory
target_include_directories(pxlFramework PRIVATE Vulkan::Headers)

target_link_libraries(pxlFramework
    glfw
    glad
    spdlog
    glm
    imgui
    stb
    assimp
    yaml-cpp
    Vulkan::Vulkan
    discordgamesdk
    Vulkan::shaderc_combined
)

target_precompile_headers(pxlFramework PUBLIC src/PCH.h)

target_compile_definitions(pxlFramework PRIVATE 
    GLFW_EXPOSE_NATIVE_WIN32
    GLFW_INCLUDE_NONE   # Tell GLFW to not include it's own OpenGL headers
    GLFW_INCLUDE_VULKAN # Tell GLFW to include the Vulkan header 
    $<$<CONFIG:Debug>:PXL_DEBUG>
    $<$<CONFIG:Release>:PXL_RELEASE>
)

# Specify source directory as an include folder for simplified '#include's in code
# TODO: This currently causes the problem that applications now have access to all source files, which isn't ideal
#       and should instead only expose pxl.h (I am unsure if this is even possible)
target_include_directories(pxlFramework PUBLIC src)

# Specify include directories for projects using pxlFramework via add_subdirectory()
target_include_directories(pxlFramework PUBLIC include)