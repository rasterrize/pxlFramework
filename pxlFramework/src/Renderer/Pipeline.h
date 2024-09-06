#pragma once

#include "Shader.h"
#include "BufferLayout.h"
#include "UniformLayout.h"

namespace pxl
{
    enum class PrimitiveTopology
    {
        None,
        Triangle,
        TriangleStrip,
        TriangleFan,
        Line,
        LineStrip,
        Point,
    };

    enum class PolygonFillMode
    {
        Fill,
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
    };

    // NOTE: Most of these members aren't even necessary for OpenGL
    struct GraphicsPipelineSpecs
    {
        PrimitiveTopology PrimitiveType = PrimitiveTopology::Triangle;
        PolygonFillMode PolygonFillMode = PolygonFillMode::Fill;
        CullMode CullMode = CullMode::None;
        BufferLayout VertexLayout;
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