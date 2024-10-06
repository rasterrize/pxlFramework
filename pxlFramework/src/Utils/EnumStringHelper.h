#pragma once

#include "Core/Image.h"
#include "Core/Input.h"
#include "Core/Window.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Camera.h"
#include "Renderer/GPUBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/RendererAPIType.h"
#include "Renderer/UniformLayout.h"

namespace pxl
{
    class EnumStringHelper
    {
    public:
        static std::string ToString(RendererAPIType type);
        static std::string ToString(GPUBufferUsage usage);
        static std::string ToString(GPUBufferDrawHint hint);
        static std::string ToString(WindowMode mode);
        static std::string ToString(CursorMode mode);
        static std::string ToString(BufferDataType type);
        static std::string ToString(UniformDataType type);
        static std::string ToString(QueueType type);
        static std::string ToString(ImageFormat format);
        static std::string ToString(ImageFilter filter);
        static std::string ToString(PrimitiveTopology topology);
        static std::string ToString(PolygonMode mode);
        static std::string ToString(CullMode mode);
        static std::string ToString(FrontFace face);
        static std::string ToString(ShaderStage stage);
    };
}