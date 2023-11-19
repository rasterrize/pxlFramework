// THIS IS A TEMPORARY FILE FOR VULKAN ERROR CHECKING, SINCE I NEED TO LEARN TO MAKE PREPROCESSOR MACROS
#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h> // should only be included in Debug builds

namespace pxl
{
    void CheckVkResult(VkResult result);
}