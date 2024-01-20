#include "VulkanContext.h"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <set>
#include <spdlog/spdlog.h>

#include "DebugUtils.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "glfw/GLFWContext.h"
#include "utility/Utility.h"

std::unique_ptr<VulkanContext> VulkanContext::ms_instance;

void VulkanContext::Initialize()
{
    ms_instance = std::unique_ptr<VulkanContext>(new VulkanContext());
}

const VulkanContext& VulkanContext::Get()
{
    ASSERT(ms_instance != nullptr && "VulkanContext has not been initialized!");
    return *ms_instance;
}

const vk::Device& VulkanContext::getLogicalDevice() const
{
    return m_logicalDevice;
}

VulkanContext::VulkanContext()
{
    init();
}

std::vector<const char *> VulkanContext::getRequiredInstanceExtensions()
{
    std::vector extensions = GLFWContext::Get().getRequiredVulkanInstanceExtensions();
    DebugUtils::AppendRequiredInstanceExtensions(extensions);

    return extensions;
}

void VulkanContext::createInstance()
{
    vk::ApplicationInfo appInfo = {
        .sType = vk::StructureType::eApplicationInfo,
        .pNext = nullptr,
        .pApplicationName = "My Vulkan App",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    auto extensions = getRequiredInstanceExtensions();

    vk::InstanceCreateInfo createInfo = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = nullptr,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if constexpr (DebugUtils::ValidationLayersEnabled())
    {
        DebugUtils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.pNext = nullptr;
    }

    std::vector<const char *> enabledLayers;
    DebugUtils::AppendInstanceLayers(enabledLayers);
    createInfo.enabledLayerCount = enabledLayers.size();
    createInfo.ppEnabledLayerNames = enabledLayers.data();

    m_instance = vk::createInstance(createInfo);
}

void VulkanContext::logSupportedInstanceExtensions()
{
    spdlog::info("supported instance extensions:");
    for (auto extensionProperties : vk::enumerateInstanceExtensionProperties())
    {
        spdlog::info("{}: version {}", extensionProperties.extensionName, extensionProperties.specVersion);
    }
}

bool VulkanContext::isDescreteGPU(const vk::PhysicalDevice& device)
{
    const auto deviceProperties = device.getProperties();
    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
}

void VulkanContext::logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices)
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

bool VulkanContext::checkDeviceExtensionsSupport(const vk::PhysicalDevice& device)
{
    std::set<std::string> requiredExtensions(m_device_extensions.begin(), m_device_extensions.end());

    for (const auto deviceExtensionProperties : device.enumerateDeviceExtensionProperties())
    {
        requiredExtensions.erase(deviceExtensionProperties.extensionName);
    }

    return requiredExtensions.empty();
}

bool VulkanContext::isDeviceSuitable(const vk::PhysicalDevice& device)
{
    const auto indices = QueueFamilyIndices::FindQueueFamilies(device, m_surface);
    const bool extensionsSupported = checkDeviceExtensionsSupport(device);
    const auto swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(device, m_surface);

    return indices.isComplete() && extensionsSupported && swapChainSupportDetails.isAdequate();
}

void VulkanContext::pickPhysicalDevice()
{
    const auto devices = m_instance.enumeratePhysicalDevices();
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
            m_physicalDevice = dev;
            return;
        }
    }

    // pick any suitable if no descrete is available
    for (const auto& dev : devices)
    {
        if (isDeviceSuitable(dev))
        {
            spdlog::warn("Descrete GPU not found! Picked {}", dev.getProperties().deviceName);
            m_physicalDevice = dev;
            return;
        }
    }

    throw std::runtime_error("Can't find suitable GPU!");
}

void VulkanContext::createLogicalDevice()
{
    const auto indices = QueueFamilyIndices::FindQueueFamilies(m_physicalDevice, m_surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set uniqueQueueFamilies = indices.getUniqueIndices();

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
        .enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size()),
        .ppEnabledExtensionNames = m_device_extensions.data(),
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

void VulkanContext::createSwapChain()
{
    auto swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(m_physicalDevice, m_surface);

    vk::SurfaceFormatKHR surfaceFormat = swapChainSupportDetails.chooseSurfaceFormat();
    vk::PresentModeKHR presentMode = swapChainSupportDetails.choosePresentMode();
    vk::Extent2D extent = swapChainSupportDetails.chooseExtent();
    vk::SurfaceCapabilitiesKHR capabilities = swapChainSupportDetails.capabilities();
    std::uint32_t imageCount = swapChainSupportDetails.chooseImageCount();

    vk::SwapchainCreateInfoKHR swapChainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = m_surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1, // this value is always 1 unless we are developing stereoscopic 3D application
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        // opaque, because we don't need blending with other windows
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(m_physicalDevice, m_surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        // specify between which queue families ownership will be transfered
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        // Exclusive mode offers better perfomance, but requires explicit ownership transfer when using multiple queue families
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    m_swapChain = m_logicalDevice.createSwapchainKHR(swapChainCreateInfo);

    m_swapChainImages = m_logicalDevice.getSwapchainImagesKHR(m_swapChain);
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

void VulkanContext::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (std::size_t i = 0; i < m_swapChainImages.size(); ++i)
    {
        const vk::ComponentMapping componentMapping = {
            .r = vk::ComponentSwizzle::eIdentity,
            .g = vk::ComponentSwizzle::eIdentity,
            .b = vk::ComponentSwizzle::eIdentity,
            .a = vk::ComponentSwizzle::eIdentity
        };

        const vk::ImageSubresourceRange imageSubresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        const vk::ImageViewCreateInfo createInfo = {
            .sType = vk::StructureType::eImageViewCreateInfo,
            .pNext = nullptr,
            .flags = {},
            .image = m_swapChainImages[i],
            .viewType = vk::ImageViewType::e2D,
            .format = m_swapChainImageFormat,
            .components = componentMapping,
            .subresourceRange = imageSubresourceRange
        };

        m_swapChainImageViews[i] = m_logicalDevice.createImageView(createInfo);
    }
}

void VulkanContext::init()
{
    createInstance();
    DebugUtils::SetupDebugMessenger(m_instance);
    logSupportedInstanceExtensions();
    GLFWContext::Get().createVulkanWindowSurface(m_instance, m_surface);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    // createGraphicsPipeline();
}

VulkanContext::~VulkanContext()
{
    cleanup();
}

void VulkanContext::cleanup()
{
    DebugUtils::Cleanup(m_instance);
    for (const auto& imageView : m_swapChainImageViews)
    {
        m_logicalDevice.destroyImageView(imageView);
    }
    m_logicalDevice.destroySwapchainKHR(m_swapChain);
    m_logicalDevice.destroy();
    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}
