#include "VulkanDevice.h"

#include "DebugUtils.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <set>

#include "VulkanContext.h"

void VulkanDevice::init(const std::vector<vk::PhysicalDevice>& availableDevices)
{
    m_physicalDevice = pickPhysicalDevice(availableDevices);
    createLogicalDevice(m_physicalDevice);
}

void VulkanDevice::destroy() noexcept
{
    m_logicalDevice.destroy();
}

const vk::Device& VulkanDevice::getLogicalDevice() const
{
    return m_logicalDevice;
}

const vk::PhysicalDevice& VulkanDevice::getPhysicalDevice() const
{
    return m_physicalDevice;
}

bool VulkanDevice::isDescreteGPU(const vk::PhysicalDevice& device)
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

bool VulkanDevice::checkDeviceExtensionsSupport(const vk::PhysicalDevice& device)
{
    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto deviceExtensionProperties : device.enumerateDeviceExtensionProperties())
    {
        requiredExtensions.erase(deviceExtensionProperties.extensionName);
    }

    return requiredExtensions.empty();
}

bool VulkanDevice::isDeviceSuitable(const vk::PhysicalDevice& device)
{
    const auto indices = QueueFamilyIndices::FindQueueFamilies(device, VulkanContext::Get().getSurface());
    const bool extensionsSupported = checkDeviceExtensionsSupport(device);
    const auto swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(device, VulkanContext::Get().getSurface());

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
    const auto indices = QueueFamilyIndices::FindQueueFamilies(m_physicalDevice, VulkanContext::Get().getSurface());
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

    vk::DeviceCreateInfo deviceCreateInfo = {
        .sType = vk::StructureType::eDeviceCreateInfo,
        .queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size()),
        .ppEnabledExtensionNames = m_deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };


    std::vector<const char *> enabledLayers;
    DebugUtils::AppendInstanceLayers(enabledLayers);
    deviceCreateInfo.enabledLayerCount = enabledLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

    m_logicalDevice = m_physicalDevice.createDevice(deviceCreateInfo);
    m_graphicsQueue = m_logicalDevice.getQueue(indices.graphicsFamily.value(), 0);
    m_presentQueue = m_logicalDevice.getQueue(indices.presentFamily.value(), 0);
}
