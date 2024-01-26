#ifndef VULKANSWAPCHAINSUPPORTDETAILS_H
#define VULKANSWAPCHAINSUPPORTDETAILS_H

#include <vulkan/vulkan.hpp>

#include "utility/Utility.h"

class VulkanSwapchainSupportDetails {
public:
    NODISCARD bool isAdequate() const;

    NODISCARD vk::SurfaceFormatKHR  chooseSurfaceFormat() const;
    NODISCARD vk::PresentModeKHR    choosePresentMode() const;
    NODISCARD vk::Extent2D          chooseExtent() const;
    NODISCARD std::uint32_t         chooseImageCount() const;

    NODISCARD vk::SurfaceCapabilitiesKHR capabilities() const;

    static VulkanSwapchainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);

private:
    vk::SurfaceCapabilitiesKHR m_capabilities;
    std::vector<vk::SurfaceFormatKHR> m_formats;
    std::vector<vk::PresentModeKHR> m_presentModes;
};



#endif //VULKANSWAPCHAINSUPPORTDETAILS_H
