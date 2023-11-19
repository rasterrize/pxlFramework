#pragma once

namespace pxl
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        virtual void* GetPipelineLayout() = 0;
    };

    // Compute Pipeline
}