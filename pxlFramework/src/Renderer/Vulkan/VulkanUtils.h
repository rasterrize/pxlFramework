#pragma once

#include <volk/volk.h>
#include <vulkan/vk_enum_string_helper.h>

#include "Renderer/GPUBuffer.h"
#include "Renderer/GraphicsDevice.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#ifdef PXL_DEBUG
    #define VK_CHECK(result) VulkanUtils::CheckVkResult(result)
#else
    #define VK_CHECK(x) x
#endif

namespace pxl
{
    namespace VulkanUtils
    {
#ifdef PXL_DEBUG
        inline void CheckVkResult(VkResult result)
        {
            if (result != VK_SUCCESS)
            {
                PXL_LOG_ERROR(LogArea::Vulkan, "Vulkan Error: {}", string_VkResult(result));
                PXL_DEBUG_BREAK;
            }
        }
#endif

        inline std::vector<VkLayerProperties> GetAvailableInstanceLayers()
        {
            uint32_t count = 0;
            VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));

            std::vector<VkLayerProperties> availableLayers(count);
            VK_CHECK(vkEnumerateInstanceLayerProperties(&count, availableLayers.data()));

            return availableLayers;
        }

        inline std::vector<VkExtensionProperties> GetAvailableInstanceExtensions(std::string layerName = "")
        {
            const char* layerChar = nullptr;
            if (!layerName.empty())
                layerChar = layerName.c_str();

            uint32_t count = 0;
            VK_CHECK(vkEnumerateInstanceExtensionProperties(layerChar, &count, nullptr));

            std::vector<VkExtensionProperties> extensions(count);
            VK_CHECK(vkEnumerateInstanceExtensionProperties(layerChar, &count, extensions.data()));

            return extensions;
        }

        inline bool ValidateExtensions(const std::vector<const char*>& requiredExtensions, const std::vector<VkExtensionProperties>& availableExtensions)
        {
            uint32_t validExtensions = 0;
            for (const auto& required : requiredExtensions)
            {
                for (const auto& available : availableExtensions)
                {
                    if (strcmp(required, available.extensionName) == 0)
                    {
                        validExtensions++;
                        break;
                    }
                }
            }

            if (validExtensions == requiredExtensions.size())
                return true;

            return false;
        }

        inline bool ValidateLayer(const char* layerName, const std::vector<VkLayerProperties>& layers)
        {
            for (const auto& layer : layers)
            {
                if (strcmp(layer.layerName, layerName) == 0)
                    return true;
            }

            return false;
        }

        inline bool ValidateExtension(const char* extensionName, const std::vector<VkExtensionProperties>& extensions)
        {
            for (const auto& extension : extensions)
            {
                if (strcmp(extension.extensionName, extensionName) == 0)
                    return true;
            }

            return false;
        }

        inline bool ValidatePresentMode(VkPresentModeKHR presentMode, const std::vector<VkPresentModeKHR>& supportedModes)
        {
            for (const auto& mode : supportedModes)
            {
                if (mode == presentMode)
                    return true;
            }

            return false;
        }

        inline std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance)
        {
            uint32_t deviceCount = 0;
            VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

            std::vector<VkPhysicalDevice> devices(deviceCount);
            VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

            return devices;
        }

        inline std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice physicalDevice)
        {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

            return queueFamilies;
        }

        inline std::vector<VkQueueFamilyProperties2> GetQueueFamilies2(VkPhysicalDevice physicalDevice)
        {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties2> queueFamilies(queueFamilyCount);

            for (auto& family : queueFamilies)
            {
                family.sType = { VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 };
            }

            vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, queueFamilies.data());

            return queueFamilies;
        }

        inline std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice physicalDevice)
        {
            uint32_t extensionCount = 0;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

            return extensions;
        }

        inline std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
            uint32_t formatCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

            std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

            return surfaceFormats;
        }

        inline std::vector<VkPresentModeKHR> GetSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
            uint32_t presentModeCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

            return presentModes;
        }

        inline VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
            VkSurfaceCapabilitiesKHR capabilities = {};
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

            return capabilities;
        }

        inline std::optional<uint32_t> GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies, VkPhysicalDevice gpu, VkSurfaceKHR surface)
        {
            std::optional<uint32_t> graphicsQueueIndex;

            // Find suitable queue families
            for (uint32_t i = 0; i < queueFamilies.size(); i++)
            {
                // Find suitable graphics queue family
                if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                {
                    // Check if this graphics queue family supports presentation to a surface
                    VkBool32 surfaceSupport;
                    vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &surfaceSupport);

                    if (surfaceSupport == VK_TRUE)
                    {
                        graphicsQueueIndex = i;
                        break;
                    }
                }
            }

            return graphicsQueueIndex;
        }

        inline VkSurfaceFormatKHR GetSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
        {
            // Select most suitable surface format
            for (const auto& surfaceFormat : surfaceFormats)
            {
                if ((surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM || surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) &&
                    surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return surfaceFormat;
            }

            // This combination is guaranteed to be available everywhere
            return { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        inline std::vector<VkImage> GetSwapchainImages(VkDevice device, VkSwapchainKHR swapchain)
        {
            uint32_t imageCount;
            VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));

            std::vector<VkImage> images(imageCount);
            VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()));

            return images;
        }

        inline VkQueue GetQueueHandle(VkDevice device, const std::optional<uint32_t>& queueIndex)
        {
            VkQueue queue = VK_NULL_HANDLE;

            if (queueIndex.has_value())
                vkGetDeviceQueue(device, queueIndex.value(), 0, &queue);

            return queue;
        }

        inline VkPhysicalDevice GetFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices)
        {
            for (const auto& gpu : physicalDevices)
            {
                VkPhysicalDeviceProperties2 props = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
                vkGetPhysicalDeviceProperties2(gpu, &props);

                if (props.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    return gpu;
            }

            return VK_NULL_HANDLE;
        }

        inline VkSemaphore CreateSemaphore(VkDevice device)
        {
            VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

            VkSemaphore semaphore;
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore));

            return semaphore;
        }

        inline VkFence CreateFence(VkDevice device, bool signaled)
        {
            VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

            if (signaled)
                fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            VkFence fence;
            VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &fence));

            return fence;
        }

        inline VkShaderModule CreateShaderModule(VkDevice device, const std::vector<uint32_t>& code)
        {
            VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
            shaderModuleCreateInfo.codeSize = code.size() * sizeof(uint32_t);
            shaderModuleCreateInfo.pCode = code.data();

            VkShaderModule shaderModule = VK_NULL_HANDLE;
            VK_CHECK(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

            return shaderModule;
        }

        inline VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code)
        {
            VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
            shaderModuleCreateInfo.codeSize = code.size();
            shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule = VK_NULL_HANDLE;
            VK_CHECK(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

            return shaderModule;
        }

        inline VkShaderStageFlagBits ToVkShaderStage(ShaderStage stage)
        {
            switch (stage)
            {
                case ShaderStage::Vertex:                 return VK_SHADER_STAGE_VERTEX_BIT;
                case ShaderStage::Fragment:               return VK_SHADER_STAGE_FRAGMENT_BIT;
                case ShaderStage::Geometry:               return VK_SHADER_STAGE_GEOMETRY_BIT;
                case ShaderStage::TessellationControl:    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                case ShaderStage::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                default:                                  return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            }
        }

        inline VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveTopology topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Triangle:      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
                case PrimitiveTopology::TriangleFan:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
                case PrimitiveTopology::Line:          return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                case PrimitiveTopology::LineStrip:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
                case PrimitiveTopology::Point:         return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                default:                               return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
            }
        }

        inline VkPolygonMode ToVkPolygonMode(PolygonMode mode)
        {
            switch (mode)
            {
                case PolygonMode::Fill:  return VK_POLYGON_MODE_FILL;
                case PolygonMode::Line:  return VK_POLYGON_MODE_LINE;
                case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
                default:                 return VK_POLYGON_MODE_MAX_ENUM;
            }
        }

        inline VkCullModeFlagBits ToVkCullMode(CullMode mode)
        {
            switch (mode)
            {
                case CullMode::None:  return VK_CULL_MODE_NONE;
                case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
                case CullMode::Back:  return VK_CULL_MODE_BACK_BIT;
                default:              return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
            }
        }

        inline VkFrontFace ToVkFrontFace(FrontFace face)
        {
            switch (face)
            {
                case FrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
                case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
                default:                          return VK_FRONT_FACE_MAX_ENUM;
            }
        }

        inline VkBufferUsageFlagBits ToVkBufferUsage(GPUBufferUsage usage)
        {
            switch (usage)
            {
                case GPUBufferUsage::Vertex:  return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                case GPUBufferUsage::Index:   return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                case GPUBufferUsage::Uniform: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                default:                      return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
            }
        }

        inline VkFormat ToVkFormat(BufferDataType type)
        {
            switch (type)
            {
                case BufferDataType::Float: return VK_FORMAT_R32_SFLOAT;
                case BufferDataType::Vec2:  return VK_FORMAT_R32G32_SFLOAT;
                case BufferDataType::Vec3:  return VK_FORMAT_R32G32B32_SFLOAT;
                case BufferDataType::Vec4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
                case BufferDataType::Int:   return VK_FORMAT_R32_SINT;
                case BufferDataType::IVec2: return VK_FORMAT_R32G32_SINT;
                case BufferDataType::IVec3: return VK_FORMAT_R32G32B32_SINT;
                case BufferDataType::IVec4: return VK_FORMAT_R32G32B32A32_SINT;
                case BufferDataType::Bool:  return VK_FORMAT_R32_SINT; // TODO: check if this is correct
                default:                    return VK_FORMAT_UNDEFINED;
            }
        }

        inline VkPhysicalDeviceType ToVkPhysicalDeviceType(GPUType type)
        {
            switch (type)
            {
                case GPUType::Discrete:   return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
                case GPUType::Integrated: return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
                default:                  return VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
            }
        }
    };
}