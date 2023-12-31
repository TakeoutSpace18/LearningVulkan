#ifndef VULKANAPP_H
#define VULKANAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


class VulkanApp
{
public:
    void run();

    const int WIDTH = 600;
    const int HEIGHT = 400;

private:
    std::vector<const char*> getRequiredInstanceExtensions();

    void createInstance();

    static void logSupportedInstanceExtensions();

    static bool isDescreteGPU(const vk::PhysicalDevice& device);

    static void logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices);

    bool checkDeviceExtensionsSupport(const vk::PhysicalDevice& device);

    bool isDeviceSuitable(const vk::PhysicalDevice& device);

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createSurface();

    void createSwapChain();

    void createImageViews();

    void initVulkan();

    void initWindow();

    void mainLoop();

    void cleanup();

    GLFWwindow* m_window = nullptr;

    vk::Instance m_instance = VK_NULL_HANDLE;
    vk::SurfaceKHR m_surface = VK_NULL_HANDLE;
    vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    vk::Device m_logicalDevice = VK_NULL_HANDLE;
    vk::SwapchainKHR m_swapChain = VK_NULL_HANDLE;
    vk::Queue m_graphicsQueue = VK_NULL_HANDLE;
    vk::Queue m_presentQueue = VK_NULL_HANDLE;

    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::ImageView> m_swapChainImageViews;

    vk::Format m_swapChainImageFormat;
    vk::Extent2D m_swapChainExtent;

    const std::vector<const char *> m_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

#endif //VULKANAPP_H
