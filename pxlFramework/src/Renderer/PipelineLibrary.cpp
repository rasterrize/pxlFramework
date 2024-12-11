#include "PipelineLibrary.h"

namespace pxl
{
    void PipelineLibrary::Add(const std::string& name, const std::shared_ptr<GraphicsPipeline>& pipeline)
    {
        s_GraphicsPipelines[name] = pipeline;
    }

    std::shared_ptr<GraphicsPipeline> PipelineLibrary::Get(const std::string& name)
    {
        return s_GraphicsPipelines.at(name);
    }
}