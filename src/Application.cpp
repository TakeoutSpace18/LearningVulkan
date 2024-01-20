#include "Application.h"

#include <spdlog/spdlog.h>

#include "glfw/GLFWContext.h"
#include "vulkan/VulkanContext.h"

void Application::run()
{
    GLFWContext::Initialize(1280, 720, "VulkanApp");
    VulkanContext::Initialize();
    mainLoop();
}

void Application::mainLoop()
{
    while (!GLFWContext::Get().appShouldClose())
    {
        GLFWContext::Get().pollEvents();
    }
}