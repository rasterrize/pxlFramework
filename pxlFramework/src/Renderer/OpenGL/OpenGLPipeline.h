#pragma once

#include "../Pipeline.h"

namespace pxl
{
    class OpenGLGraphicsPipeline : public GraphicsPipeline
    {
    public:
        OpenGLGraphicsPipeline(const std::shared_ptr<Shader>& shader)
         : m_Shader(shader) {}
        virtual ~OpenGLGraphicsPipeline() = default;

        virtual void Bind() override;
        virtual void SetPushConstantData(std::unordered_map<std::string, const void*>& pcData) override { 
            PXL_LOG_WARN(LogArea::OpenGL, "Setting push constant data for OpenGL isn't possible"); 
        }

        virtual void* GetPipelineLayout() override { return nullptr; }
    private:
        std::shared_ptr<Shader> m_Shader;
    };
}