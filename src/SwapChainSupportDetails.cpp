#include "SwapChainSupportDetails.h"

#include <cstdint>
#include <limits>
#include <algorithm>

#include <GLFW/glfw3.h>

bool SwapChainSupportDetails::isAdequate() const
{
    return !m_formats.empty() && !m_presentModes.empty();
}

vk::SurfaceFormatKHR SwapChainSupportDetails::chooseSurfaceFormat() const
{
    for (const auto& availableFormat : m_formats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace ==
            vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return m_formats.at(0);
}

vk::PresentModeKHR SwapChainSupportDetails::choosePresentMode() const
{
    for (const auto& availablePresentMode : m_presentModes)
    {
        // Mailbox mode is desired because it provides minimum latency but still without tearing
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
    }

    // FIFO mode is guaranteed to be available
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChainSupportDetails::chooseExtent(GLFWwindow* window) const
{
    if (m_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return m_capabilities.currentExtent;
    }

    // some window managers allow us to specify extent ourselves,
    // so we need to retrieve it from glfw.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, m_capabilities.minImageExtent.width,
                                    m_capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, m_capabilities.minImageExtent.height,
                                     m_capabilities.maxImageExtent.height);

    return actualExtent;
}

std::uint32_t SwapChainSupportDetails::chooseImageCount() const
{
    // requesting exact minimum may sometimes cause us to wait on the driver
    // to complete internal operations before we can acquire another image to render to.
    // So we request one more image.
    std::uint32_t imageCount = m_capabilities.minImageCount + 1;

    // zero value indicates that there is no maximum
    if (m_capabilities.maxImageCount > 0 && imageCount > m_capabilities.maxImageCount)
    {
        imageCount = m_capabilities.maxImageCount;
    }

    return imageCount;
}

vk::SurfaceCapabilitiesKHR SwapChainSupportDetails::capabilities() const
{
    return m_capabilities;
}

SwapChainSupportDetails SwapChainSupportDetails::QuerySwapChainSupport(const vk::PhysicalDevice& device,
                                                                       const vk::SurfaceKHR& surface)
{
    SwapChainSupportDetails details;
    details.m_capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.m_formats = device.getSurfaceFormatsKHR(surface);
    details.m_presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}
