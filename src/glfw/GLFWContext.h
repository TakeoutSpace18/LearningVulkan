#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include <string>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "utility/NonCopyable.h"
#include "utility/NonMovable.h"
#include "utility/Utility.h"

struct GLFWwindow;

class GLFWContext : NonCopyable, NonMovable {
public:

    static void Initialize(int width, int height, const std::string& title);
    static GLFWContext& Get();

    ~GLFWContext();

    std::vector<const char*> getRequiredVulkanInstanceExtensions();

    NODISCARD bool appShouldClose() const;

    NODISCARD std::pair<int, int> getFrameBufferSize();

    void createVulkanWindowSurface(vk::Instance instance, vk::SurfaceKHR& surface) const;

    void pollEvents();

private:
    GLFWContext() = default;

    GLFWwindow* m_window = nullptr;

    static GLFWContext s_instance;
};



#endif //GLFWWINDOW_H
