#pragma once

#include "GPUResource.h"

namespace pxl
{
    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
    };

    struct ShaderSpecs
    {
        ShaderStage Stage;
        std::filesystem::path FilePath;
    };

    /// @brief A program that executes code at specific stages within a pipeline
    class Shader : public GPUResource
    {
    public:
        Shader(const ShaderSpecs& specs)
            : m_Specs(specs)
        {
        }
        virtual ~Shader() = default;

        virtual void Free() override = 0;

        /// @brief Reloads this shader from disk
        /// @note  Any pipelines using this shader must be recreated manually
        virtual void Reload() = 0;

        ShaderSpecs GetSpecs() const { return m_Specs; }

    protected:
        ShaderSpecs m_Specs;
    };
}