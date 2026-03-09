#pragma once

#include "BufferLayout.h"
#include "GPUResource.h"
#include "Shader.h"

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
        Clockwise,
        CounterClockwise,
    };

    struct GraphicsPipelineSpecs
    {
        std::unordered_map<ShaderStage, std::shared_ptr<Shader>> Shaders;
        BufferLayout BufferLayout;
        PrimitiveTopology PrimitiveTopology = PrimitiveTopology::Triangle;
        PolygonMode PolygonMode = PolygonMode::Fill;
        CullMode CullMode = CullMode::None;
        FrontFace FrontFace = FrontFace::CounterClockwise;
        // std::optional<UniformLayout> UniformLayout;
    };

    class GraphicsPipeline : public GPUResource
    {
    public:
        GraphicsPipeline(const GraphicsPipelineSpecs& specs)
            : m_Specs(specs)
        {
        }
        virtual ~GraphicsPipeline() = default;

        virtual void Recreate() = 0;

        virtual void Free() override = 0;

        const GraphicsPipelineSpecs& GetSpecs() { return m_Specs; }

    protected:
        GraphicsPipelineSpecs m_Specs;
    };
}