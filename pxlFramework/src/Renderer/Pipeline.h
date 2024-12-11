#pragma once

#include "BufferLayout.h"
#include "Shader.h"
#include "UniformLayout.h"

namespace pxl
{
    enum class PrimitiveTopology
    {
        Triangle,
        TriangleStrip,
        TriangleFan,
        Line,
        LineStrip,
        Point,
    };

    enum class PolygonMode
    {
        Fill,
        Line,
        Point,
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
    };

    enum class FrontFace
    {
        CW,  // Clockwise
        CCW, // Counter-clockwise
    };

    // NOTE: Most of these members aren't even necessary for OpenGL
    struct GraphicsPipelineSpecs
    {
        std::unordered_map<ShaderStage, std::shared_ptr<Shader>> Shaders;
        BufferLayout VertexLayout;
        PrimitiveTopology PrimitiveType = PrimitiveTopology::Triangle;
        PolygonMode PolygonMode = PolygonMode::Fill;
        CullMode CullMode = CullMode::None;
        FrontFace FrontFace = FrontFace::CCW;
        std::optional<UniformLayout> UniformLayout;
        std::optional<PushConstantLayout> PushConstantLayout;
    };

    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetUniformData(const std::string& name, UniformDataType type, const void* data) = 0;
        virtual void SetUniformData(const std::string& name, UniformDataType type, uint32_t count, const void* data) = 0;

        virtual void SetPushConstantData(const std::string& name, const void* data) = 0;

        virtual void* GetPipelineLayout() = 0;

        static std::shared_ptr<GraphicsPipeline> Create(const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders);
    };

    // Compute Pipeline
}