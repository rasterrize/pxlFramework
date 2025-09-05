#pragma once

// Include Windows.h before anything else, to avoid confusing preprocessor warnings
#ifdef _WIN32
    #include <Windows.h>
#endif

// pxlFramework
#include "Core/Assert.h"
#include "Core/Logging/FrameworkLog.h"
#include "Core/Size.h"
#include "Debug/Profiler.h"

// std
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <thread>