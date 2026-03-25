#include "ShaderCompiler.h"

namespace pxl
{
    std::vector<uint32_t> ShaderCompiler::CompileGLSLToSPIRV(ShaderStage stage, const std::string& shaderSource, const std::string& shaderFileName)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        bool optimize = true;
        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        auto module = compiler.CompileGlslToSpv(shaderSource, ShadercUtils::ToShadercKind(stage), shaderFileName.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            PXL_LOG_ERROR(LogArea::Renderer, "Shaderc compilation error: {}", module.GetErrorMessage());
            return std::vector<uint32_t>();
        }

        PXL_LOG_INFO(LogArea::Renderer, "Successfully compiled shader '{}'", shaderFileName);

        return { module.cbegin(), module.cend() };
    }
}