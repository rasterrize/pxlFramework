#pragma once

#include "BufferLayout.h"
#include "GPUResource.h"
#include "Shader.h"
#include "TextureHandler.h"
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
        BufferLayout BufferLayout = {};
        PrimitiveTopology PrimitiveTopology = PrimitiveTopology::Triangle;
        PolygonMode PolygonMode = PolygonMode::Fill;
        CullMode CullMode = CullMode::None;
        FrontFace FrontFace = FrontFace::CounterClockwise;
        std::optional<UniformLayout> UniformLayout;
        std::shared_ptr<TextureHandler> TextureHandler;
    };

    /// @brief A pipeline representing the various stages geometry data will pass through.
    class GraphicsPipeline : public GPUResource
    {
    public:
        virtual ~GraphicsPipeline() = default;

        /// @brief Recreates the pipeline using its predefined specifications.
        virtual void Recreate() = 0;

        /// @brief Sets the cull mode. No recreation is needed for this.
        virtual void SetCullMode(CullMode mode) = 0;

        /// @brief Sets the front face. No recreation is needed for this.
        virtual void SetFrontFace(FrontFace face) = 0;

        /// @brief Sets the primitive topology. No recreation is needed for this.
        virtual void SetPrimitiveTopology(PrimitiveTopology topology) = 0;

        virtual const GraphicsPipelineSpecs& GetSpecs() const = 0;
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