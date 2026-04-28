#pragma once

// std
#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

// NOTE: These files may use stl files, so always include them after
// clang-format off
// pxlFramework
#include "Core/Assert.h"
#include "Core/Logging/FrameworkLog.h"
#include "Debug/Profiler.h"
#include "Core/Size.h"
// clang-format on