#ifndef SWAPCHAINSUPPORTDETAILS_H
#define SWAPCHAINSUPPORTDETAILS_H

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>


class SwapChainSupportDetails {
public:
    [[nodiscard]] bool isAdequate() const;

    [[nodiscard]] vk::SurfaceFormatKHR  chooseSurfaceFormat() const;
    [[nodiscard]] vk::PresentModeKHR    choosePresentMode() const;
    [[nodiscard]] vk::Extent2D          chooseExtent(GLFWwindow* window) const;

    static SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);

private:
    vk::SurfaceCapabilitiesKHR m_capabilities;
    std::vector<vk::SurfaceFormatKHR> m_formats;
    std::vector<vk::PresentModeKHR> m_presentModes;
};



#endif //SWAPCHAINSUPPORTDETAILS_H
