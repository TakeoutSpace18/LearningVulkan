#include "DebugUtils.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vk_extension_helper.h>
#include <set>
#include <spdlog/spdlog.h>

#include "VulkanContext.h"

VkBool32 DebugUtils::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                   void* pUserData)
{
    switch (messageSeverity)
    {
        case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT):
            spdlog::warn(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            spdlog::debug(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            spdlog::info(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            spdlog::error(pCallbackData->pMessage);
            break;
        default:
            spdlog::error("Unknown severity flag in VK debug callback!");
    }
    return VK_FALSE;
}

bool DebugUtils::CheckValidationLayerSupport()
{
    std::set<std::string> requiredLayers(s_validation_layers.begin(), s_validation_layers.end());

    for (const auto& instanceLayerProperties : vk::enumerateInstanceLayerProperties())
    {
        requiredLayers.erase(instanceLayerProperties.layerName);
    }

    return requiredLayers.empty();
}

void DebugUtils::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = vk::DebugUtilsMessengerCreateInfoEXT{};
    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                             vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                             vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                             vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
    createInfo.pfnUserCallback = debugCallback;
}

void DebugUtils::SetupDebugMessenger(vk::Instance instance)
{
    if constexpr (!ValidationLayersEnabled()) return;

    const vk::DispatchLoaderDynamic dldi(instance, vkGetInstanceProcAddr);
    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    s_debug_messenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
}

void DebugUtils::Cleanup()
{
    const vk::Instance& instance = VulkanContext::Get().getVulkanInstance();
    const vk::DispatchLoaderDynamic dldi(instance, vkGetInstanceProcAddr);
    instance.destroyDebugUtilsMessengerEXT(s_debug_messenger, nullptr, dldi);
}

void DebugUtils::AppendRequiredInstanceExtensions(std::vector<const char *>& instanceExtensions)
{
    if constexpr (ValidationLayersEnabled())
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

void DebugUtils::AppendInstanceLayers(std::vector<const char*>& instanceLayers)
{
    if constexpr (ValidationLayersEnabled())
    {
        if (!CheckValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        for (const char* layer : s_validation_layers)
        {
            instanceLayers.push_back(layer);
        }
    }
}