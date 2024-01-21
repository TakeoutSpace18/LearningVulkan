#include "GLFWContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include "utility/Utility.h"

GLFWContext GLFWContext::s_instance;

GLFWContext& GLFWContext::Get()
{
    ASSERT(s_instance.m_window != nullptr && "GLFWwindow has not been created!");
    return s_instance;
}

std::vector<const char *> GLFWContext::getRequiredVulkanInstanceExtensions()
{
    std::uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    return {extensions, extensions + count};
}

bool GLFWContext::appShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

std::pair<int, int> GLFWContext::getFrameBufferSize()
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {width, height};
}

void GLFWContext::createVulkanWindowSurface(vk::Instance instance, vk::SurfaceKHR& surface) const
{
    auto cStyleSurface = static_cast<VkSurfaceKHR>(surface);

    if (glfwCreateWindowSurface(instance, m_window, nullptr, &cStyleSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create window surface!");
    }
    surface = cStyleSurface;
}

void GLFWContext::pollEvents()
{
    glfwPollEvents();
}

GLFWContext::~GLFWContext()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void GLFWContext::Initialize(int width, int height, const std::string& title)
{
    glfwSetErrorCallback([](int code, const char* desc) {
        spdlog::error("GLFW Error {}: {}", code, desc);
    });

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    s_instance.m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}
