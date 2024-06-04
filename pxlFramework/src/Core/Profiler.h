#pragma once

#ifdef PXL_ENABLE_PROFILING
    #include "tracy/Tracy.hpp"

    #define PXL_PROFILE_SCOPE ZoneScoped;
    #define PXL_PROFILE_FRAME_END FrameMark;
#else
    #define PXL_PROFILE_SCOPE
    #define PXL_PROFILE_FRAME_END
#endif