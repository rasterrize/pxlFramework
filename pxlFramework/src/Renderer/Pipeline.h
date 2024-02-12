#pragma once

#include "Shader.h"
#include "BufferLayout.h"

namespace pxl
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        virtual void Bind() = 0;

        virtual void* GetPipelineLayout() = 0;
        virtual void Destroy() = 0;

        static std::shared_ptr<GraphicsPipeline> Create(const std::shared_ptr<Shader>& shader, const BufferLayout& vertexLayout);
    };

    // Compute Pipeline
}