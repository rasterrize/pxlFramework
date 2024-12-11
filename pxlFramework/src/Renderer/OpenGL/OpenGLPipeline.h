#pragma once

#include <glad/glad.h>

#include "OpenGLShader.h"
#include "Renderer/Pipeline.h"

namespace pxl
{
    class OpenGLGraphicsPipeline : public GraphicsPipeline
    {
    public:
        OpenGLGraphicsPipeline(const GraphicsPipelineSpecs& specs);
        virtual ~OpenGLGraphicsPipeline() = default;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetUniformData(const std::string& name, UniformDataType type, const void* data) override;
        virtual void SetUniformData(const std::string& name, UniformDataType type, uint32_t count, const void* data) override;
        virtual void SetPushConstantData([[maybe_unused]] const std::string& name, [[maybe_unused]] const void* data) override
        {
            PXL_LOG_WARN(LogArea::OpenGL, "Can't set push constant data, OpenGL doesn't support push constants");
        }

        virtual void* GetLayout() override { return nullptr; }

        virtual const GraphicsPipelineSpecs& GetSpecs() override { return m_Specs; }
        virtual void SetSpecs(const GraphicsPipelineSpecs& specs) { m_Specs = specs; }

    private:
        int GetUniformLocation(const std::string& name) const;

        GLenum ToGLCullMode(CullMode mode);
        GLenum ToGLPolygonMode(PolygonMode mode);
        GLenum ToGLFrontFace(FrontFace face);

    private:
        uint32_t m_ShaderProgramID = 0;

        std::unordered_map<ShaderStage, std::shared_ptr<Shader>> m_Shaders;

        mutable std::unordered_map<std::string, int> m_UniformCache;

        GraphicsPipelineSpecs m_Specs = {};

        bool m_CullingEnabled = true;
    };
}