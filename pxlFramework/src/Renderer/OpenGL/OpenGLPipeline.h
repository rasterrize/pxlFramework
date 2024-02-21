#pragma once

#include "../Pipeline.h"

namespace pxl
{
    class OpenGLGraphicsPipeline : public GraphicsPipeline
    {
    public:
        OpenGLGraphicsPipeline(const std::shared_ptr<Shader>& shader);
        virtual ~OpenGLGraphicsPipeline() = default;

        virtual void Bind() override;

        virtual void* GetPipelineLayout() override { return nullptr; }
        virtual void Destroy() override {};
    private:
        std::shared_ptr<Shader> m_Shader;
        //std::shared_ptr<BufferLayout> m_BufferLayout;

    };
}