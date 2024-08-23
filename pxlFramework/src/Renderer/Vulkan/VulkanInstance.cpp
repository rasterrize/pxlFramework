#include "VulkanInstance.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VkInstance VulkanInstance::s_Instance = nullptr;

    void VulkanInstance::Init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
    {
        if (s_Instance)
        {
            PXL_LOG_WARN(LogArea::Vulkan, "Vulkan instance already initialized");
            return;
        }
        
        // Check vulkan API version
        [[maybe_unused]] auto apiVersion = VulkanHelpers::GetVulkanAPIVersion();

        // TODO: check if the implementation api version is greater than the specifed application version

        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_3; // The vulkan API version this application (code base) is built on (not implementation version)

        VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceInfo.ppEnabledExtensionNames = extensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        instanceInfo.ppEnabledLayerNames = layers.data();

        VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &s_Instance));

        PXL_ASSERT_MSG(s_Instance, "Failed to create Vulkan instance");

        VulkanDeletionQueue::Add([&]() {
            vkDestroyInstance(s_Instance, nullptr);
            s_Instance = VK_NULL_HANDLE;
        });

        // Logging 
        #ifdef PXL_ENABLE_LOGGING
        
        std::string apiVersionString;
        switch (appInfo.apiVersion)
        {
            case VK_API_VERSION_1_0:
                apiVersionString = "1.0";
                break;
            case VK_API_VERSION_1_1:
                apiVersionString = "1.1";
                break;
            case VK_API_VERSION_1_2:
                apiVersionString = "1.2";
                break;
            case VK_API_VERSION_1_3:
                apiVersionString = "1.3";
                break;
        }

        PXL_LOG_INFO(LogArea::Vulkan, "VK Instance Info:");
        PXL_LOG_INFO(LogArea::Vulkan, "   Application Vulkan API Version: {}", apiVersionString);

        PXL_LOG_INFO(LogArea::Vulkan, "   {} enabled extensions: ", extensions.size());
        for (const auto& extension : extensions)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "   - {}", extension);
        }

        PXL_LOG_INFO(LogArea::Vulkan, "   {} enabled layers:", layers.size());
        for (const auto& layer : layers)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "   - {}", layer);
        }

        #endif
    }
}