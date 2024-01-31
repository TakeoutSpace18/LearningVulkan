#include "VulkanContext.h"

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include <spdlog/spdlog.h>

#include "VulkanDebugUtils.h"
#include "glfw/GLFWContext.h"
#include "utility/Utility.h"

std::unique_ptr<VulkanContext> VulkanContext::s_instance;

void VulkanContext::Initialize()
{
    s_instance = std::unique_ptr<VulkanContext>(new VulkanContext());
    s_instance->init();
}

VulkanContext& VulkanContext::Get()
{
    ASSERT(s_instance != nullptr && "VulkanContext has not been initialized!");
    return *s_instance;
}

vk::Device VulkanContext::GetLogicalDevice()
{
    return GetDevice().getLogicalDevice();
}

vk::PhysicalDevice VulkanContext::GetPhysicalDevice()
{
    return GetDevice().getPhysicalDevice();
}

vk::Instance VulkanContext::GetVulkanInstance()
{
    return Get().m_instance;
}

vk::SurfaceKHR VulkanContext::GetSurface()
{
    return Get().m_surface;
}

VulkanSwapchain& VulkanContext::GetSwapchain()
{
    return Get().m_swapchain;
}

VulkanDevice& VulkanContext::GetDevice()
{
    return Get().m_device;
}

void VulkanContext::DrawFrame()
{
    Get().m_renderPipeline.drawFrame();
}

std::vector<const char *> VulkanContext::getRequiredInstanceExtensions()
{
    std::vector extensions = GLFWContext::Get().getRequiredVulkanInstanceExtensions();
    VulkanDebugUtils::AppendRequiredInstanceExtensions(extensions);

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
    if constexpr (VulkanDebugUtils::ValidationLayersEnabled())
    {
        VulkanDebugUtils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else
    {
        createInfo.pNext = nullptr;
    }

    std::vector<const char *> enabledLayers;
    VulkanDebugUtils::AppendInstanceLayers(enabledLayers);
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
    VulkanDebugUtils::SetupDebugMessenger(m_instance);
    LogSupportedInstanceExtensions();
    GLFWContext::Get().createVulkanWindowSurface(m_instance, m_surface);
    m_device.init(m_instance.enumeratePhysicalDevices());
    m_swapchain.init();
    m_renderPipeline.init();
}

VulkanContext::~VulkanContext() noexcept
{
    cleanup();
}

void VulkanContext::cleanup() noexcept
{
    VulkanDebugUtils::Cleanup();
    m_renderPipeline.destroy();
    m_swapchain.destroy();
    m_device.destroy();
    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}
