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
        Compute,
    };

    struct ShaderSpecs
    {
        /// @brief The stage this shader accommodates in a pipeline
        ShaderStage Stage;

        /// @brief Compiled SPIRV code
        std::vector<uint32_t> Code;

        /// @brief File path to the shader's source code
        std::filesystem::path SourcePath;
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
        /// @return Whether or not reloading was successful, use this to determine whether
        /// to recreate the graphics pipeline or not
        /// @note Any pipelines using this shader must be recreated manually for any visible changes
        virtual bool Reload(const std::vector<uint32_t>& code) = 0;

        ShaderSpecs GetSpecs() const { return m_Specs; }
        void SetSpecs(const ShaderSpecs& specs) { m_Specs = specs; }

    protected:
        ShaderSpecs m_Specs;
    };

    namespace Utils
    {
        inline std::string ToString(ShaderStage stage)
        {
            switch (stage)
            {
                case ShaderStage::Vertex:                 return "Vertex";
                case ShaderStage::Fragment:               return "Fragment";
                case ShaderStage::Geometry:               return "Geometry";
                case ShaderStage::TessellationControl:    return "TessellationControl";
                case ShaderStage::TessellationEvaluation: return "TessellationEvaluation";
                case ShaderStage::Compute:                return "Compute";
                default:                                  return "Unknown";
            }
        }
    }
}