#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vulkan/vulkan.hpp>

#include "utility/Utility.h"

class VulkanDevice {
public:
    struct DeviceQueues
    {
        vk::Queue graphicsQueue = VK_NULL_HANDLE;
        vk::Queue presentQueue = VK_NULL_HANDLE;
    };

public:
    void init(const std::vector<vk::PhysicalDevice>& availableDevices);
    void destroy() noexcept;

    NODISCARD vk::Device getLogicalDevice() const;
    NODISCARD vk::PhysicalDevice getPhysicalDevice() const;
    NODISCARD const DeviceQueues& getQueues() const;

private:
    static bool isDescreteGPU(vk::PhysicalDevice device);

    static void logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices);

    static void logPhysicalDeviceInfo(vk::PhysicalDevice device);

    bool checkDeviceExtensionsSupport(vk::PhysicalDevice device);

    bool isDeviceSuitable(vk::PhysicalDevice device);

    vk::PhysicalDevice pickPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices);

    void createLogicalDevice(vk::PhysicalDevice physicalDevice);

private:
    vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    vk::Device m_logicalDevice = VK_NULL_HANDLE;
    DeviceQueues m_queues;

    const std::vector<const char *> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
};



#endif //VULKANDEVICE_H
