#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

class VulkanSwapchain {
public:

    void init();
    void destroy() noexcept;

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
