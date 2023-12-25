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

void VulkanApp::createInstance()
{
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
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = glfw_extension_count,
        .ppEnabledExtensionNames = glfw_extensions
    };

    m_vk_instance = vk::createInstance(create_info);

}

void VulkanApp::initVulkan()
{
    createInstance();

    fmt::print(stderr, "supported instance extensions:\n");
    for (auto ext_props :  vk::enumerateInstanceExtensionProperties())
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
