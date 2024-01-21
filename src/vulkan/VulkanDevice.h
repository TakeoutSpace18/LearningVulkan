#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "utility/Utility.h"

class VulkanDevice {
public:

    void init(const std::vector<vk::PhysicalDevice>& availableDevices);
    void destroy() noexcept;

    NODISCARD const vk::Device& getLogicalDevice() const;
    NODISCARD const vk::PhysicalDevice& getPhysicalDevice() const;

private:
    static bool isDescreteGPU(const vk::PhysicalDevice& device);

    static void logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices);

    bool checkDeviceExtensionsSupport(const vk::PhysicalDevice& device);

    bool isDeviceSuitable(const vk::PhysicalDevice& device);

    vk::PhysicalDevice pickPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices);

    void createLogicalDevice(vk::PhysicalDevice physicalDevice);

private:
    vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    vk::Device m_logicalDevice = VK_NULL_HANDLE;

    vk::Queue m_graphicsQueue = VK_NULL_HANDLE;
    vk::Queue m_presentQueue = VK_NULL_HANDLE;

    const std::vector<const char *> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};



#endif //VULKANDEVICE_H
