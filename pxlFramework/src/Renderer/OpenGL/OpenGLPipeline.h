#pragma once

#include <glad/glad.h>

#include "OpenGLShader.h"
#include "Renderer/Pipeline.h"

namespace pxl
{
    class OpenGLGraphicsPipeline : public GraphicsPipeline
    {
    public:
        OpenGLGraphicsPipeline(const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders);
        virtual ~OpenGLGraphicsPipeline() = default;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetUniformData(const std::string& name, UniformDataType type, const void* data) override;
        virtual void SetUniformData(const std::string& name, UniformDataType type, uint32_t count, const void* data) override;
        virtual void SetPushConstantData([[maybe_unused]] const std::string& name, [[maybe_unused]] const void* data) override
        {
            PXL_LOG_WARN(LogArea::OpenGL, "Can't set push constant data, OpenGL doesn't support push constants");
        }

        virtual void* GetPipelineLayout() override { return nullptr; }
    private:
        int GetUniformLocation(const std::string& name) const;

        GLenum ToGLCullMode(CullMode mode);
        GLenum ToGLPolygonMode(PolygonMode mode);
        GLenum ToGLFrontFace(FrontFace face);
    private:
        uint32_t m_ShaderProgramID = 0;
        mutable std::unordered_map<std::string, int> m_UniformCache;

        bool m_CullingEnabled = true;
        GLenum m_PolygonMode = GL_FILL;
        GLenum m_CullMode = GL_BACK;
        GLenum m_FrontFace = GL_CCW;
    };
}