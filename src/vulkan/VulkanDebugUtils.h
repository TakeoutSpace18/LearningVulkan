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
        return s_enableValidationLayers;
    }

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    static bool CheckLayerSupport(const char* requiredLayerName);

private:
    inline static vk::DebugUtilsMessengerEXT s_debug_messenger = VK_NULL_HANDLE;

    inline const static std::vector<const char *> s_validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };

#ifdef NDEBUG
    static constexpr bool s_enableValidationLayers = false;
#else
    static constexpr bool s_enableValidationLayers = true;
#endif

    static constexpr bool s_enableMesaOverlay = true;
    inline static const char* s_MesaOverlayLayerName = "VK_LAYER_MESA_overlay";
};


#endif //VULKANDEBUGUTILS_H
