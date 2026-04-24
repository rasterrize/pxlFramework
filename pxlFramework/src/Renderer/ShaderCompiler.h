#pragma once

#include <shaderc/shaderc.hpp>

#include "Shader.h"

namespace pxl
{
    class ShaderCompiler
    {
    public:
        std::vector<uint32_t> CompileGLSLToSPIRV(ShaderStage stage, const std::string& shaderSource, const std::string& shaderFileName);

    private:
        shaderc::Compiler m_Compiler = {};
    };

    namespace ShadercUtils
    {
        inline shaderc_shader_kind ToShadercKind(ShaderStage stage)
        {
            switch (stage)
            {
                case ShaderStage::Vertex:                 return shaderc_vertex_shader;
                case ShaderStage::Fragment:               return shaderc_fragment_shader;
                case ShaderStage::Geometry:               return shaderc_geometry_shader;
                case ShaderStage::TessellationControl:    return shaderc_tess_control_shader;
                case ShaderStage::TessellationEvaluation: return shaderc_tess_evaluation_shader;
                default:                                  return shaderc_vertex_shader;
            }
        }
    }
}