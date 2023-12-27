#ifndef VULKANAPP_H
#define VULKANAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <optional>
#include <vulkan/vulkan.hpp>


struct QueueFamilyIndices {
    std::optional<std::uint32_t> graphicsFamily;

    [[nodiscard]] bool isComplete() const;
};

class VulkanApp
{
public:
    void run();

    const int WIDTH = 600;
    const int HEIGHT = 400;

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    bool checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    void createInstance();

    static void logSupportedInstanceExtensions();

    static bool isDescreteGPU(const vk::PhysicalDevice& device);

    static void logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices);

    bool isDeviceSuitable(const vk::PhysicalDevice& device);

    void pickPhysicalDevice();

    QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);

    void createLogicalDevice();

    void initVulkan();

    void initWindow();

    void mainLoop();

    void cleanup();

    GLFWwindow* m_window = nullptr;
    vk::Instance m_vk_instance = VK_NULL_HANDLE;
    vk::PhysicalDevice m_physical_device = VK_NULL_HANDLE;
    vk::Device m_logical_device = VK_NULL_HANDLE;
    vk::Queue m_graphics_queue = VK_NULL_HANDLE;

    const std::vector<const char *> m_validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    static constexpr bool m_enable_validation_layers = false;
#else
    static constexpr bool m_enable_validation_layers = true;
#endif
};

#endif //VULKANAPP_H
