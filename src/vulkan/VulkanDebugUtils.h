#ifndef VULKANDEBUGUTILS_H
#define VULKANDEBUGUTILS_H

#include <vector>
#include <vulkan/vulkan.hpp>

class VulkanDebugUtils
{
public:
    static void AppendRequiredInstanceExtensions(std::vector<const char *>& instanceExtensions);

    static void AppendInstanceLayers(std::vector<const char *>& instanceLayers);

    static void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    static void SetupDebugMessenger(vk::Instance instance);

    static void Cleanup();

    static constexpr bool ValidationLayersEnabled()
    {
        return m_enable_validation_layers;
    }

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    static bool CheckValidationLayerSupport();

private:
    inline static vk::DebugUtilsMessengerEXT s_debug_messenger = VK_NULL_HANDLE;

    inline const static std::vector<const char *> s_validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    static constexpr bool m_enable_validation_layers = false;
#else
    static constexpr bool m_enable_validation_layers = true;
#endif
};


#endif //VULKANDEBUGUTILS_H
