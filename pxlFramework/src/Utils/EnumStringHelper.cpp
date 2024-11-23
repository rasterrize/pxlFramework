#include "EnumStringHelper.h"

namespace pxl
{
    std::string EnumStringHelper::ToString(RendererAPIType type)
    {
        switch (type)
        {
            case RendererAPIType::None:   return "None";
            case RendererAPIType::OpenGL: return "OpenGL";
            case RendererAPIType::Vulkan: return "Vulkan";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(GPUBufferUsage usage)
    {
        switch (usage)
        {
            case GPUBufferUsage::None:    return "None";
            case GPUBufferUsage::Vertex:  return "Vertex";
            case GPUBufferUsage::Index:   return "Index";
            case GPUBufferUsage::Uniform: return "Uniform";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(GPUBufferDrawHint hint)
    {
        switch (hint)
        {
            case GPUBufferDrawHint::Static:  return "Static";
            case GPUBufferDrawHint::Dynamic: return "Dynamic";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(WindowMode mode)
    {
        switch (mode)
        {
            case WindowMode::Windowed:   return "Windowed";
            case WindowMode::Borderless: return "Borderless";
            case WindowMode::Fullscreen: return "Fullscreen";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(CursorMode mode)
    {
        switch (mode)
        {
            case CursorMode::Normal:   return "Normal";
            case CursorMode::Hidden:   return "Hidden";
            case CursorMode::Disabled: return "Disabled";
            case CursorMode::Captured: return "Captured";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(BufferDataType type)
    {
        switch (type)
        {
            case BufferDataType::None:   return "None";
            case BufferDataType::Float:  return "Float";
            case BufferDataType::Float2: return "Float2";
            case BufferDataType::Float3: return "Float3";
            case BufferDataType::Float4: return "Float4";
            case BufferDataType::Int:    return "Int";
            case BufferDataType::Int2:   return "Int2";
            case BufferDataType::Int3:   return "Int3";
            case BufferDataType::Int4:   return "Int4";
            case BufferDataType::Mat3:   return "Mat3";
            case BufferDataType::Mat4:   return "Mat4";
            case BufferDataType::Bool:   return "Bool";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(UniformDataType type)
    {
        switch (type)
        {
            case UniformDataType::None:     return "None";
            case UniformDataType::Float:    return "Float";
            case UniformDataType::Float2:   return "Float2";
            case UniformDataType::Float3:   return "Float3";
            case UniformDataType::Float4:   return "Float4";
            case UniformDataType::Int:      return "Int";
            case UniformDataType::Int2:     return "Int2";
            case UniformDataType::Int3:     return "Int3";
            case UniformDataType::Int4:     return "Int4";
            case UniformDataType::Mat3:     return "Mat3";
            case UniformDataType::Mat4:     return "Mat4";
            case UniformDataType::Bool:     return "Bool";
            case UniformDataType::IntArray: return "IntArray";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(QueueType type)
    {
        switch (type)
        {
            case QueueType::Graphics: return "Graphics";
            case QueueType::Compute:  return "Compute";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined: return "Undefined";
            case ImageFormat::RGB8:      return "RGB8";
            case ImageFormat::RGBA8:     return "RGBA8";
        }

        return "Invalid";
    }

    std::string EnumStringHelper::ToString(SampleFilter filter)
    {
        switch (filter)
        {
            case SampleFilter::Undefined: return "Undefined";
            case SampleFilter::Nearest:   return "Nearest";
            case SampleFilter::Linear:    return "Linear";
        }

        return "Invalid";
    }

    std::string EnumStringHelper::ToString(PrimitiveTopology topology)
    {
        switch (topology)
        {
            case PrimitiveTopology::Triangle:      return "Triangle";
            case PrimitiveTopology::TriangleStrip: return "TriangleStrip";
            case PrimitiveTopology::TriangleFan:   return "TriangleFan";
            case PrimitiveTopology::Line:          return "Line";
            case PrimitiveTopology::LineStrip:     return "LineStrip";
            case PrimitiveTopology::Point:         return "Point";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(PolygonMode mode)
    {
        switch (mode)
        {
            case PolygonMode::Fill:  return "Fill";
            case PolygonMode::Line:  return "Line";
            case PolygonMode::Point: return "Point";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None:  return "None";
            case CullMode::Front: return "Front";
            case CullMode::Back:  return "Back";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(FrontFace face)
    {
        switch (face)
        {
            case FrontFace::CW:  return "Clockwise";
            case FrontFace::CCW: return "Counter-Clockwise";
        }

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:       return "Vertex";
            case ShaderStage::Fragment:     return "Fragment";
            case ShaderStage::Geometry:     return "Geometry";
            case ShaderStage::Tessellation: return "Tessellation";
        }

        return "Undefined";
    }
}