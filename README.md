# pxlFramework
### A Windows C++ Application/Game Framework
This is a hobby project I've been casually working on. Originally, it started as just me following TheCherno's [Game Engine series](https://youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&si=15wnEV42qfltyt2v) but at some point I realized I was more or less copying the code and not fully comprehending the reasons behind things. So I decided I would make all the decisions and write all the future code instead of just copying the tutorial. This is why some aspects of the code may look similar to TheCherno's game engine, such as the structure of renderer classes.

**NOTE:** This project is still very **WIP**. There are bugs, and things WILL crash. I have a lot of plans for it, so much that it may never be 'complete'.

### Features
#### Currently implemented features
- 2D/3D graphics using orthographic and perspective cameras
- OpenGL and Vulkan rendering backends
- Simple window creation and handling
- Batched rendering (_Quads, cubes, lines, meshes_)
- File loading (_Images, models, audio, etc_)
- Keyboard and mouse input handling
- Fast and simple console logging
- ImGui support (_GUI for debugging_)
- Code profiling
- Framework configuration file

#### Planned features
- Multithreading
- Controller input handling support
- Basic audio playback
- Instanced rendering
- Optimization

### Building/using pxlFramework
This project uses **CMake** along with a **C++20** compiler. However right now I only use MSVC, so I'm unsure if it works for with any other compilers.

If you wish to create an application or game with this framework... well atm good luck because there isn't any documentation or template yet :P

To build you will need:
 - [CMake](https://cmake.org/download/) (_any recent version_)
 - [VulkanSDK](https://vulkan.lunarg.com/sdk/home) (_with **Shader Toolchain Debug Symbols x64** and **Vulkan Memory Allocator header** checked_)
 - [MSVC](https://visualstudio.microsoft.com/downloads/) (_any version supporting **C++20**_)

To clone use:
```sh
git clone https://github.com/rasterrize/pxlFramework
```

#### Dependencies/Libraries this project uses
- [GLFW](https://github.com/glfw/glfw) (for window and input handling)
- [Glad](https://glad.dav1d.de/) (for OpenGL)
- [VulkanSDK](https://vulkan.lunarg.com/sdk/home) (for Vulkan)
- [GLM](https://github.com/g-truc/glm) (for maths)
- [ImGui](https://github.com/ocornut/imgui) (for GUI debugging)
- [spdlog](https://github.com/gabime/spdlog) (for logging)
- [stb](https://github.com/nothings/stb) (for image loading/writing)
- [BASS](https://www.un4seen.com/) (for audio playback)
- [assimp](https://github.com/assimp/assimp) (for model loading)
- [Tracy](https://github.com/wolfpld/tracy) (for profiling)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) (for config serialization)
- [Discord Game SDK](https://discord.com/developers/docs/developer-tools/game-sdk) (for discord rpc)

### buh  
![](https://media.tenor.com/Hx5yUAxxYvwAAAAM/buh-b-u-h.gif)