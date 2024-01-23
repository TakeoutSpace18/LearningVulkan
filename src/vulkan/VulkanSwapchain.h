#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "utility/Utility.h"

class VulkanSwapchain {
public:
    void init();
    void destroy() noexcept;

    NODISCARD vk::Extent2D getExtent() const;
    NODISCARD vk::ImageView getImageView(std::uint32_t index) const;
    NODISCARD vk::SwapchainKHR getHandle() const;

    std::uint32_t acquireNextImage(std::uint64_t timeout, vk::Semaphore semaphore, vk::Fence fence);

private:
    void createSwapChain();
    void createImageViews();

private:
    vk::SwapchainKHR m_swapChain = VK_NULL_HANDLE;

    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::ImageView> m_swapChainImageViews;

    vk::Format m_swapChainImageFormat;
    vk::Extent2D m_swapChainExtent;
};



#endif //VULKANSWAPCHAIN_H
