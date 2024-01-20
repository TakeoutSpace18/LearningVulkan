#ifndef VULKANAPP_H
#define VULKANAPP_H

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <memory>

#include "utility/NonCopyable.h"
#include "utility/NonMovable.h"
#include "utility/Utility.h"

// TODO: extract VulkanDevice class from here
class VulkanContext : NonCopyable, NonMovable
{
public:

    ~VulkanContext();

    static void Initialize();
    static const VulkanContext& Get();

    NODISCARD const vk::Device& getLogicalDevice() const;

private:
    VulkanContext();

    void init();

    void cleanup();

    std::vector<const char*> getRequiredInstanceExtensions();

    void createInstance();

    static void logSupportedInstanceExtensions();

    static bool isDescreteGPU(const vk::PhysicalDevice& device);

    static void logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices);

    bool checkDeviceExtensionsSupport(const vk::PhysicalDevice& device);

    bool isDeviceSuitable(const vk::PhysicalDevice& device);

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createSwapChain();

    void createImageViews();

    void createGraphicsPipeline();

private:
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

    static std::unique_ptr<VulkanContext> ms_instance;
};

#endif //VULKANAPP_H
