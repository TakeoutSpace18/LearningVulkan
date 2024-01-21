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

std::unique_ptr<VulkanContext> VulkanContext::s_instance;

void VulkanContext::Initialize()
{
    s_instance = std::unique_ptr<VulkanContext>(new VulkanContext());
    s_instance->init();
}

const VulkanContext& VulkanContext::Get()
{
    ASSERT(s_instance != nullptr && "VulkanContext has not been initialized!");
    return *s_instance;
}

const vk::Device& VulkanContext::getLogicalDevice() const
{
    return m_device.getLogicalDevice();
}

const vk::PhysicalDevice& VulkanContext::getPhysicalDevice() const
{
    return m_device.getPhysicalDevice();
}

const vk::Instance& VulkanContext::getVulkanInstance() const
{
    return m_instance;
}

const vk::SurfaceKHR& VulkanContext::getSurface() const
{
    return m_surface;
}

const VulkanSwapchain& VulkanContext::getSwapchain() const
{
    return m_swapchain;
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

void VulkanContext::LogSupportedInstanceExtensions()
{
    spdlog::info("supported instance extensions:");
    for (auto extensionProperties : vk::enumerateInstanceExtensionProperties())
    {
        spdlog::info("{}: version {}", extensionProperties.extensionName, extensionProperties.specVersion);
    }
}

void VulkanContext::init()
{
    createInstance();
    DebugUtils::SetupDebugMessenger(m_instance);
    LogSupportedInstanceExtensions();
    GLFWContext::Get().createVulkanWindowSurface(m_instance, m_surface);
    m_device.init(m_instance.enumeratePhysicalDevices());
    m_swapchain.init();
}

VulkanContext::~VulkanContext()
{
    cleanup();
}

void VulkanContext::cleanup()
{
    DebugUtils::Cleanup();
    m_swapchain.destroy();
    m_device.destroy();
    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}
