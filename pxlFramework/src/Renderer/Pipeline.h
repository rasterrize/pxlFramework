#pragma once

#include "Shader.h"
#include "BufferLayout.h"
#include "UniformLayout.h"

namespace pxl
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        virtual void Bind() = 0;
        virtual void SetPushConstantData(std::unordered_map<std::string, const void*>& pcData) = 0;

        virtual void* GetPipelineLayout() = 0;
        virtual void Destroy() = 0;

        static std::shared_ptr<GraphicsPipeline> Create(const std::shared_ptr<Shader>& shader);
        static std::shared_ptr<GraphicsPipeline> Create(const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders, const BufferLayout& vertexLayout, const UniformLayout& uniformLayout);
    };

    // Compute Pipeline
}