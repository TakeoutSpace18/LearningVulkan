#include "VulkanDebugUtils.h"

#include <vulkan/vk_enum_string_helper.h>
#include <set>
#include <spdlog/spdlog.h>

#include "VulkanContext.h"

VkBool32 VulkanDebugUtils::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

bool VulkanDebugUtils::CheckLayerSupport(const char* requiredLayerName)
{
    for (const auto& instanceLayerProperties : vk::enumerateInstanceLayerProperties())
    {
        if (std::strcmp(instanceLayerProperties.layerName, requiredLayerName) == 0)
        {
            return true;
        }
    }

    return false;
}

void VulkanDebugUtils::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
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

void VulkanDebugUtils::SetupDebugMessenger(vk::Instance instance)
{
    if constexpr (!ValidationLayersEnabled()) return;

    const vk::DispatchLoaderDynamic dldi(instance, vkGetInstanceProcAddr);
    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    s_debug_messenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
}

void VulkanDebugUtils::Cleanup()
{
    const vk::Instance& instance = VulkanContext::GetVulkanInstance();
    const vk::DispatchLoaderDynamic dldi(instance, vkGetInstanceProcAddr);
    instance.destroyDebugUtilsMessengerEXT(s_debug_messenger, nullptr, dldi);
}

void VulkanDebugUtils::AppendRequiredInstanceExtensions(std::vector<const char *>& instanceExtensions)
{
    if constexpr (ValidationLayersEnabled())
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

void VulkanDebugUtils::AppendInstanceLayers(std::vector<const char *>& instanceLayers)
{
    if constexpr (ValidationLayersEnabled())
    {
            for (const char* layer : s_validation_layers)
            {
                instanceLayers.push_back(layer);
                if (CheckLayerSupport(layer))
                {
                    instanceLayers.push_back(layer);
                }
                else
                {
                    spdlog::error("Layer {} is not available!", layer);
                }
            }
    }

    if constexpr (s_enableMesaOverlay)
    {
        if (CheckLayerSupport(s_MesaOverlayLayerName))
        {
            instanceLayers.push_back(s_MesaOverlayLayerName);
        }
        else
        {
            spdlog::error("Layer {} is not available!", s_MesaOverlayLayerName);
        }
    }
}
