#ifndef SWAPCHAINSUPPORTDETAILS_H
#define SWAPCHAINSUPPORTDETAILS_H

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "utility/Utility.h"

class SwapChainSupportDetails {
public:
    NODISCARD bool isAdequate() const;

    NODISCARD vk::SurfaceFormatKHR  chooseSurfaceFormat() const;
    NODISCARD vk::PresentModeKHR    choosePresentMode() const;
    NODISCARD vk::Extent2D          chooseExtent() const;
    NODISCARD std::uint32_t         chooseImageCount() const;

    NODISCARD vk::SurfaceCapabilitiesKHR capabilities() const;

    static SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);

private:
    vk::SurfaceCapabilitiesKHR m_capabilities;
    std::vector<vk::SurfaceFormatKHR> m_formats;
    std::vector<vk::PresentModeKHR> m_presentModes;
};



#endif //SWAPCHAINSUPPORTDETAILS_H
