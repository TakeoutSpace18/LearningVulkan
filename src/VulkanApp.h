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

    void initVulkan();

    void initWindow();

    void mainLoop();

    void cleanup();

    GLFWwindow* m_window = nullptr;

    vk::Instance m_vk_instance = VK_NULL_HANDLE;
    vk::SurfaceKHR m_surface_khr = VK_NULL_HANDLE;
    vk::PhysicalDevice m_physical_device = VK_NULL_HANDLE;
    vk::Device m_logical_device = VK_NULL_HANDLE;
    vk::SwapchainKHR m_swap_chain = VK_NULL_HANDLE;
    vk::Queue m_graphics_queue = VK_NULL_HANDLE;
    vk::Queue m_present_queue = VK_NULL_HANDLE;

    std::vector<vk::Image> m_swap_chain_images;

    vk::Format m_swap_chain_image_format;
    vk::Extent2D m_swap_chain_extent;

    const std::vector<const char *> m_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

#endif //VULKANAPP_H
