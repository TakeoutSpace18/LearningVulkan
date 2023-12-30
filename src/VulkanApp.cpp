#include "VulkanApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <set>
#include <vulkan/vulkan.hpp>

#include <spdlog/spdlog.h>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

void VulkanApp::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

VkBool32 VulkanApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                  void* pUserData)
{
    switch (messageSeverity)
    {
        case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT):
            spdlog::warn("validation layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            spdlog::debug("validation layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            spdlog::info("validation layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            spdlog::error("validation layer: {}", pCallbackData->pMessage);
            break;
        default:
            spdlog::error("Unknown severity flag in VK debug callback!");
    }
    return VK_FALSE;
}

bool VulkanApp::checkValidationLayerSupport()
{
    std::set<std::string> requiredLayers(m_validation_layers.begin(), m_validation_layers.end());

    for (const auto& instanceLayerProperties : vk::enumerateInstanceLayerProperties())
    {
        requiredLayers.erase(instanceLayerProperties.layerName);
    }

    return requiredLayers.empty();
}

void VulkanApp::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = vk::DebugUtilsMessengerCreateInfoEXT{};
    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                             vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                             vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                             vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanApp::createInstance()
{
    if (m_enable_validation_layers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo = {
        .sType = vk::StructureType::eApplicationInfo,
        .pNext = nullptr,
        .pApplicationName = "My Vulkan App",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    std::uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vk::InstanceCreateInfo createInfo = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = nullptr,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions
    };

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if constexpr (m_enable_validation_layers)
    {
        populateDebugMessengerCreateInfo(debugCreateInfo);

        createInfo.enabledLayerCount = m_validation_layers.size();
        createInfo.ppEnabledLayerNames = m_validation_layers.data();
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

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

bool VulkanApp::isDeviceSuitable(const vk::PhysicalDevice& device)
{
    if (!findQueueFamilies(device).isComplete())
    {
        return false;
    }

    return true;
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

    if constexpr (m_enable_validation_layers)
    {
        deviceCreateInfo.enabledLayerCount = m_validation_layers.size();
        deviceCreateInfo.ppEnabledLayerNames = m_validation_layers.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }

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

void VulkanApp::initVulkan()
{
    createInstance();
    logSupportedInstanceExtensions();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
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
    m_logical_device.destroy();
    m_vk_instance.destroySurfaceKHR(m_surface_khr);
    m_vk_instance.destroy();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
