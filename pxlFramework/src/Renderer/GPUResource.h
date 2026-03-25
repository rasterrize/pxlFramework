#pragma once

namespace pxl
{
    /// @brief A resource allocated on the GPU.
    class GPUResource
    {
    public:
        virtual void Free() = 0;
    };
}