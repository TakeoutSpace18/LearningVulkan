#include "VulkanDevice.h"

#include <set>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanDebugUtils.h"
#include "VulkanQueueFamilyIndices.h"
#include "VulkanSwapchainSupportDetails.h"
#include "VulkanContext.h"


void VulkanDevice::init(const std::vector<vk::PhysicalDevice>& availableDevices)
{
    m_physicalDevice = pickPhysicalDevice(availableDevices);
    logPhysicalDeviceInfo(m_physicalDevice);
    createLogicalDevice(m_physicalDevice);
    createVmaAllocator();
    createCommandPool();
}

void VulkanDevice::destroy() noexcept
{
    m_logicalDevice.destroyCommandPool(m_commandPool);
    vmaDestroyAllocator(m_vmaAllocator);
    m_logicalDevice.destroy();
}

vk::Device VulkanDevice::getLogicalDevice() const
{
    return m_logicalDevice;
}

vk::PhysicalDevice VulkanDevice::getPhysicalDevice() const
{
    return m_physicalDevice;
}

vk::CommandPool VulkanDevice::getCommandPool() const
{
    return m_commandPool;
}

const VulkanDevice::DeviceQueues& VulkanDevice::getQueues() const
{
    return m_queues;
}

VmaAllocator VulkanDevice::getVmaAllocator() const
{
    ASSERT(m_vmaAllocator != VK_NULL_HANDLE && "Vulkan memory allocator is not yet initialized!");
    return m_vmaAllocator;
}

bool VulkanDevice::isDescreteGPU(const vk::PhysicalDevice device)
{
    const auto deviceProperties = device.getProperties();
    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
}

void VulkanDevice::logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices)
{
    spdlog::info("Available physical devices:");
    int number = 0;
    for (const auto& dev : devices)
    {
        auto deviceProperties = dev.getProperties();
        spdlog::info("{}) {}, ID: {}", number, deviceProperties.deviceName, deviceProperties.deviceID);
        ++number;
    }
}

void VulkanDevice::logPhysicalDeviceInfo(vk::PhysicalDevice device)
{
    vk::PhysicalDeviceProperties props = device.getProperties();
    spdlog::info("---------- {} ----------", props.deviceName);
    spdlog::info("\tType: {}",  string_VkPhysicalDeviceType(static_cast<VkPhysicalDeviceType>(props.deviceType)));
    spdlog::info("\tAPI version: {}.{}.{}",
        VK_VERSION_MAJOR(props.apiVersion),
        VK_VERSION_MINOR(props.apiVersion),
        VK_VERSION_PATCH(props.apiVersion));
}

bool VulkanDevice::checkDeviceExtensionsSupport(const vk::PhysicalDevice device)
{
    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto deviceExtensionProperties : device.enumerateDeviceExtensionProperties())
    {
        requiredExtensions.erase(deviceExtensionProperties.extensionName);
    }

    return requiredExtensions.empty();
}

bool VulkanDevice::isDeviceSuitable(const vk::PhysicalDevice device)
{
    const auto indices = VulkanQueueFamilyIndices::FindQueueFamilies(device, VulkanContext::GetSurface());
    const bool extensionsSupported = checkDeviceExtensionsSupport(device);
    const auto swapChainSupportDetails = VulkanSwapchainSupportDetails::QuerySwapChainSupport(device, VulkanContext::GetSurface());

    return indices.isComplete() && extensionsSupported && swapChainSupportDetails.isAdequate();
}

vk::PhysicalDevice VulkanDevice::pickPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices)
{
    if (devices.empty())
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    logAvailablePhysicalDevices(devices);

    // pick discrete gpu on the first hand
    for (const auto& dev : devices)
    {
        if (isDescreteGPU(dev) && isDeviceSuitable(dev))
        {
            spdlog::info("Picked descrete GPU {}", dev.getProperties().deviceName);
            return dev;
        }
    }

    // pick any suitable if no descrete is available
    for (const auto& dev : devices)
    {
        if (isDeviceSuitable(dev))
        {
            spdlog::warn("Descrete GPU not found! Picked {}", dev.getProperties().deviceName);
            return dev;
        }
    }

    throw std::runtime_error("Can't find suitable GPU!");
}

void VulkanDevice::createLogicalDevice(const vk::PhysicalDevice physicalDevice)
{
    const auto indices = VulkanQueueFamilyIndices::FindQueueFamilies(m_physicalDevice, VulkanContext::GetSurface());
    std::set<std::uint32_t> uniqueQueueFamilies = indices.getUniqueIndices();

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo{
            .sType = vk::StructureType::eDeviceQueueCreateInfo,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::PhysicalDeviceVulkan13Features deviceVulkan13Features;
    deviceVulkan13Features.dynamicRendering = VK_TRUE;

    vk::DeviceCreateInfo deviceCreateInfo = {
        .sType = vk::StructureType::eDeviceCreateInfo,
        .pNext = &deviceVulkan13Features,
        .queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size()),
        .ppEnabledExtensionNames = m_deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };


    std::vector<const char *> enabledLayers;
    VulkanDebugUtils::AppendInstanceLayers(enabledLayers);
    deviceCreateInfo.enabledLayerCount = enabledLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

    m_logicalDevice = m_physicalDevice.createDevice(deviceCreateInfo);
    m_queues.graphicsQueue = m_logicalDevice.getQueue(indices.graphicsFamily.value(), 0);
    m_queues.presentQueue = m_logicalDevice.getQueue(indices.presentFamily.value(), 0);
}

void VulkanDevice::createVmaAllocator()
{
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    const VmaAllocatorCreateInfo createInfo = {
        .flags = VmaAllocatorCreateFlags(), // add extensions here if ones are used
        .physicalDevice = m_physicalDevice,
        .device = m_logicalDevice,
        .pVulkanFunctions = &vulkanFunctions,
        .instance = VulkanContext::GetVulkanInstance(),
        .vulkanApiVersion = VK_API_VERSION_1_3,
    };

    VkResult result = vmaCreateAllocator(&createInfo, &m_vmaAllocator);
    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan memory allocator!");
}

void VulkanDevice::createCommandPool()
{
    VulkanQueueFamilyIndices queueIndices = VulkanQueueFamilyIndices::FindQueueFamilies(
        VulkanContext::GetPhysicalDevice(),
        VulkanContext::GetSurface()
    );

    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = vk::StructureType::eCommandPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queueIndices.graphicsFamily.value()
    };

    m_commandPool = VulkanContext::GetLogicalDevice().createCommandPool(commandPoolCreateInfo);
}
