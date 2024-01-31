#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vk_mem_alloc.h>
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
    NODISCARD vk::CommandPool getCommandPool() const;
    NODISCARD const DeviceQueues& getQueues() const;
    NODISCARD VmaAllocator getVmaAllocator() const;

private:
    static bool isDescreteGPU(vk::PhysicalDevice device);

    static void logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices);

    static void logPhysicalDeviceInfo(vk::PhysicalDevice device);

    bool checkDeviceExtensionsSupport(vk::PhysicalDevice device);

    bool isDeviceSuitable(vk::PhysicalDevice device);

    vk::PhysicalDevice pickPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices);

    void createLogicalDevice(vk::PhysicalDevice physicalDevice);

    void createCommandPool();

    void createVmaAllocator();

private:
    vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    vk::Device m_logicalDevice = VK_NULL_HANDLE;
    vk::CommandPool m_commandPool = VK_NULL_HANDLE;
    DeviceQueues m_queues;

    VmaAllocator m_vmaAllocator = VK_NULL_HANDLE;

    const std::vector<const char *> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
};



#endif //VULKANDEVICE_H
