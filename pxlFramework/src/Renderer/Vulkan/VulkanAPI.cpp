#include "VulkanAPI.h"

#include "Core/Window.h"
#include "VulkanGraphicsContext.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtils.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace pxl
{
    VulkanAPI::VulkanAPI()
    {
        if (volkInitialize())
            throw std::runtime_error("Failed to initialize volk");

        auto availableExtensions = VulkanUtils::GetAvailableInstanceExtensions();
        auto requiredExtensions = Window::GetVKRequiredInstanceExtensions();

        auto availableLayers = VulkanUtils::GetAvailableInstanceLayers();
        std::vector<const char*> requestedLayers;

#if defined(PXL_DEBUG) && defined(PXL_ENABLE_LOGGING)
        const char* validationLayer = "VK_LAYER_KHRONOS_validation";
        if (VulkanUtils::ValidateLayer(validationLayer, availableLayers))
        {
            requestedLayers.push_back(validationLayer);
        }
        else
        {
            PXL_LOG_WARN("Vulkan validation layer NOT found");
        }

        bool useDebugUtils = false;
        if (VulkanUtils::ValidateExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, availableExtensions))
        {
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            useDebugUtils = true;
        }
        else
        {
            PXL_LOG_WARN("Debug utils extension NOT found");
        }

        PXL_LOG_INFO("Required instance extensions selected:");
        for (const auto& extensionName : requiredExtensions)
        {
            PXL_LOG_INFO("- {}", extensionName);
        }

        PXL_LOG_INFO("Requested instance layers selected:");
        for (const auto& layerName : requestedLayers)
        {
            PXL_LOG_INFO("- {}", layerName);
        }
#endif

        if (!VulkanUtils::ValidateExtensions(requiredExtensions, availableExtensions))
            throw std::runtime_error("Required instance extensions not found");

        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.pApplicationName = "pxlFramework App";
        appInfo.pEngineName = "pxlFramework";
        appInfo.apiVersion = VK_API_VERSION_1_3; // The vulkan API version this code base is built on

        VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(requestedLayers.size());
        instanceInfo.ppEnabledLayerNames = requestedLayers.data();

#if defined(PXL_DEBUG) && defined(PXL_ENABLE_LOGGING)
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        if (useDebugUtils)
        {
            debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

            debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            debugMessengerInfo.pfnUserCallback = DebugCallback;

            instanceInfo.pNext = &debugMessengerInfo;
        }
#endif

        VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &m_Instance));
        volkLoadInstance(m_Instance);

#if defined(PXL_DEBUG) && defined(PXL_ENABLE_LOGGING)
        if (useDebugUtils)
        {
            VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerInfo, nullptr, &m_DebugMessenger));
            PXL_LOG_INFO("Vulkan debug utils messenger created");
        }
#endif
    }

    VulkanAPI::~VulkanAPI()
    {
        if (m_DebugMessenger)
        {
            vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
            m_DebugMessenger = VK_NULL_HANDLE;
        }

        if (m_Instance)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }
    }

    std::unique_ptr<GraphicsContext> VulkanAPI::CreateGraphicsContext(const GraphicsContextSpecs& specs)
    {
        return std::make_unique<VulkanGraphicsContext>(specs);
    }

    std::unique_ptr<GraphicsDevice> VulkanAPI::CreateGraphicsDevice(const GraphicsDeviceSpecs& specs)
    {
        return std::make_unique<VulkanGraphicsDevice>(specs, m_Instance);
    }

    VkBool32 VulkanAPI::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, [[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*)
    {
        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            PXL_LOG_ERROR("Validation Layer - Error: {} - {}", callbackData->pMessageIdName, callbackData->pMessage);
        }
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            PXL_LOG_WARN("Validation Layer - Warning: {} - {}", callbackData->pMessageIdName, callbackData->pMessage);
        }
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            PXL_LOG_INFO("Validation Layer - Info: {} - {}", callbackData->pMessageIdName, callbackData->pMessage);
        }
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
            PXL_LOG_INFO("Validation Layer - Verbose: {} - {}", callbackData->pMessageIdName, callbackData->pMessage);
        }

        // We must return false, true is reserved for in layer development
        return VK_FALSE;
    }
}