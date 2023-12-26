#pragma once

namespace pxl
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        //virtual void Bind() = 0;

        virtual void* GetPipelineLayout() = 0;
        virtual void Destroy() = 0;
    };

    // Compute Pipeline
}