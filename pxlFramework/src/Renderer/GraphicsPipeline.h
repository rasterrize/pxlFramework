#pragma once

#include "BufferLayout.h"
#include "GPUResource.h"
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
        std::optional<UniformLayout> UniformLayout;
    };

    /// @brief A pipeline representing the various stages geometry data will pass through.
    class GraphicsPipeline : public GPUResource
    {
    public:
        GraphicsPipeline(const GraphicsPipelineSpecs& specs)
            : m_Specs(specs)
        {
        }

        virtual ~GraphicsPipeline() = default;

        /// @brief Recreates the pipeline using its predefined specifications.
        virtual void Recreate() = 0;

        virtual void Free() override = 0;

        const GraphicsPipelineSpecs& GetSpecs() const { return m_Specs; }

    protected:
        GraphicsPipelineSpecs m_Specs;
    };

    namespace Utils
    {
        inline std::string ToString(PrimitiveTopology topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Triangle:      return "Triangle";
                case PrimitiveTopology::TriangleStrip: return "TriangleStrip";
                case PrimitiveTopology::TriangleFan:   return "TriangleFan";
                case PrimitiveTopology::Line:          return "Line";
                case PrimitiveTopology::LineStrip:     return "LineStrip";
                case PrimitiveTopology::Point:         return "Point";
                default:                               return "Unknown";
            }
        }

        inline std::string ToString(PolygonMode mode)
        {
            switch (mode)
            {
                case PolygonMode::Fill:  return "Fill";
                case PolygonMode::Line:  return "Line";
                case PolygonMode::Point: return "Point";
                default:                 return "Unknown";
            }
        }

        inline std::string ToString(CullMode mode)
        {
            switch (mode)
            {
                case CullMode::None:  return "None";
                case CullMode::Front: return "Front";
                case CullMode::Back:  return "Back";
                default:              return "Unknown";
            }
        }

        inline std::string ToString(FrontFace face)
        {
            switch (face)
            {
                case FrontFace::Clockwise:        return "Clockwise";
                case FrontFace::CounterClockwise: return "Counter-Clockwise";
                default:                          return "Unknown";
            }
        }
    }
}