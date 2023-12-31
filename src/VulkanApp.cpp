#include "VulkanApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <set>
#include <spdlog/spdlog.h>

#include "DebugUtils.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

void VulkanApp::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

std::vector<const char *> VulkanApp::getRequiredInstanceExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    DebugUtils::AppendRequiredInstanceExtensions(extensions);

    return extensions;
}

void VulkanApp::createInstance()
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

    m_vk_instance = vk::createInstance(createInfo);
}

void VulkanApp::logSupportedInstanceExtensions()
{
    spdlog::info("supported instance extensions:");
    for (auto extensionProperties : vk::enumerateInstanceExtensionProperties())
    {
        spdlog::info("{}: version {}", extensionProperties.extensionName, extensionProperties.specVersion);
    }
}

bool VulkanApp::isDescreteGPU(const vk::PhysicalDevice& device)
{
    const auto deviceProperties = device.getProperties();
    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
}

void VulkanApp::logAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices)
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

bool VulkanApp::checkDeviceExtensionsSupport(const vk::PhysicalDevice& device)
{
    std::set<std::string> requiredExtensions(m_device_extensions.begin(), m_device_extensions.end());

    for (const auto deviceExtensionProperties : device.enumerateDeviceExtensionProperties())
    {
        requiredExtensions.erase(deviceExtensionProperties.extensionName);
    }

    return requiredExtensions.empty();
}

bool VulkanApp::isDeviceSuitable(const vk::PhysicalDevice& device)
{
    const auto indices = QueueFamilyIndices::FindQueueFamilies(device, m_surface_khr);
    const bool extensionsSupported = checkDeviceExtensionsSupport(device);
    const auto swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(device, m_surface_khr);

    return indices.isComplete() && extensionsSupported && swapChainSupportDetails.isAdequate();
}

void VulkanApp::pickPhysicalDevice()
{
    const auto devices = m_vk_instance.enumeratePhysicalDevices();
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
            m_physical_device = dev;
            return;
        }
    }

    // pick any suitable if no descrete is available
    for (const auto& dev : devices)
    {
        if (isDeviceSuitable(dev))
        {
            spdlog::warn("Descrete GPU not found! Picked {}", dev.getProperties().deviceName);
            m_physical_device = dev;
            return;
        }
    }

    throw std::runtime_error("Can't find suitable GPU!");
}

void VulkanApp::createLogicalDevice()
{
    const auto indices = QueueFamilyIndices::FindQueueFamilies(m_physical_device, m_surface_khr);

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

    m_logical_device = m_physical_device.createDevice(deviceCreateInfo);

    m_graphics_queue = m_logical_device.getQueue(indices.graphicsFamily.value(), 0);
    m_present_queue = m_logical_device.getQueue(indices.presentFamily.value(), 0);
}

void VulkanApp::createSurface()
{
    auto surf = static_cast<VkSurfaceKHR>(m_surface_khr);
    if (glfwCreateWindowSurface(m_vk_instance, m_window, nullptr, &surf) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create window surface!");
    }
    m_surface_khr = surf;
}

void VulkanApp::createSwapChain()
{
    auto swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(m_physical_device, m_surface_khr);

    vk::SurfaceFormatKHR surfaceFormat = swapChainSupportDetails.chooseSurfaceFormat();
    vk::PresentModeKHR presentMode = swapChainSupportDetails.choosePresentMode();
    vk::Extent2D extent = swapChainSupportDetails.chooseExtent(m_window);
    vk::SurfaceCapabilitiesKHR capabilities = swapChainSupportDetails.capabilities();
    std::uint32_t imageCount = swapChainSupportDetails.chooseImageCount();

    vk::SwapchainCreateInfoKHR swapChainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = m_surface_khr,
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

    QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(m_physical_device, m_surface_khr);
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

    m_swap_chain = m_logical_device.createSwapchainKHR(swapChainCreateInfo);

    m_swap_chain_images = m_logical_device.getSwapchainImagesKHR(m_swap_chain);
    m_swap_chain_image_format = surfaceFormat.format;
    m_swap_chain_extent = extent;
}

void VulkanApp::initVulkan()
{
    createInstance();
    DebugUtils::SetupDebugMessenger(m_vk_instance);
    logSupportedInstanceExtensions();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
}

void VulkanApp::initWindow()
{
    glfwSetErrorCallback([](int code, const char* desc) {
        spdlog::error("GLFW Error {}: {}", code, desc);
    });

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", nullptr, nullptr);
}

void VulkanApp::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void VulkanApp::cleanup()
{
    DebugUtils::Cleanup(m_vk_instance);
    m_logical_device.destroySwapchainKHR(m_swap_chain);
    m_logical_device.destroy();
    m_vk_instance.destroySurfaceKHR(m_surface_khr);
    m_vk_instance.destroy();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
