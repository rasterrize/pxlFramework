#pragma once

#define BASS_CHECK(func) if (!func) { PXL_LOG_ERROR(LogArea::Audio, "BASS audio error: {}", BASS_ErrorGetCode()); }