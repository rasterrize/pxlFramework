#include "VulkanErrorCheck.h"

namespace pxl
{
    void CheckVkResult(VkResult result) // this function shouldnt be here (we need it for every vulkan class)
    {
        if (result == VK_SUCCESS)
		    return;

        Logger::LogError("VkResult wasn't VK_SUCCESS, error code is " + std::string(string_VkResult(result)));

	    if (result < 0)
		    abort(); // probably shouldn't abort immediately
    }
}