#pragma once

#include "Pipeline.h"

namespace pxl
{
    // A class used to store all the application's created pipelines
    class PipelineLibrary
    {
    public:
        // NOTE: only considers graphics pipelines for now
        static void Add(const std::string& name, const std::shared_ptr<GraphicsPipeline>& pipeline);

        static std::shared_ptr<GraphicsPipeline> Get(const std::string& name);
        static const std::unordered_map<std::string, std::shared_ptr<GraphicsPipeline>>& GetAll() { return s_GraphicsPipelines; }

    private:
        static inline std::unordered_map<std::string, std::shared_ptr<GraphicsPipeline>> s_GraphicsPipelines;
    };
}