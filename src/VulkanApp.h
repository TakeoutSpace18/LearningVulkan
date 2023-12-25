#ifndef VULKANAPP_H
#define VULKANAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


class VulkanApp {
public:
    void run();

    const int WIDTH = 600;
    const int HEIGHT = 400;

private:

    void createInstance();

    void initVulkan();

    void initWindow();

    void mainLoop();

    void cleanup();

    GLFWwindow* m_window;
    vk::Instance m_vk_instance;
};



#endif //VULKANAPP_H
