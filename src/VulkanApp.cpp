#include "VulkanApp.h"

#include <fmt/printf.h>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

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
    fmt::print(stderr, "validation layer: {}\n", pCallbackData->pMessage);

    return VK_FALSE;
}

bool VulkanApp::checkValidationLayerSupport()
{
    for (const char* layer_name : m_validation_layers)
    {
        bool layer_found = false;

        for (const auto& layer_properties : vk::enumerateInstanceLayerProperties())
        {
            if (strcmp(layer_name, layer_properties.layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }

        if (!layer_found)
        {
            return false;
        }
    }
    return true;
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

    vk::ApplicationInfo app_info = {
        .sType = vk::StructureType::eApplicationInfo,
        .pNext = nullptr,
        .pApplicationName = "My Vulkan App",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    std::uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    vk::InstanceCreateInfo create_info = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = nullptr,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = glfw_extension_count,
        .ppEnabledExtensionNames = glfw_extensions
    };

    vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
    if constexpr (m_enable_validation_layers)
    {
        populateDebugMessengerCreateInfo(debug_create_info);

        create_info.enabledLayerCount = m_validation_layers.size();
        create_info.ppEnabledLayerNames = m_validation_layers.data();
        create_info.pNext = &debug_create_info;
    }
    else
    {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    m_vk_instance = vk::createInstance(create_info);
}

void VulkanApp::initVulkan()
{
    createInstance();

    fmt::print(stderr, "supported instance extensions:\n");
    for (auto ext_props : vk::enumerateInstanceExtensionProperties())
    {
        fmt::print(stderr, "{}: version {}\n", ext_props.extensionName, ext_props.specVersion);
    }
}

void VulkanApp::initWindow()
{
    glfwSetErrorCallback([](int code, const char* desc) {
        fmt::print("GLFW Error {}: {}", code, desc);
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
    m_vk_instance.destroy();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
