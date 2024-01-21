#include "VulkanSwapchain.h"

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "VulkanContext.h"

void VulkanSwapchain::init()
{
    createSwapChain();
    createImageViews();
}

void VulkanSwapchain::destroy() noexcept
{
    const vk::Device logicalDevice = VulkanContext::Get().getLogicalDevice();

    for (const auto& imageView : m_swapChainImageViews)
    {
        logicalDevice.destroyImageView(imageView);
    }
    logicalDevice.destroySwapchainKHR(m_swapChain);
}

vk::Extent2D VulkanSwapchain::getExtent() const
{
    return m_swapChainExtent;
}

void VulkanSwapchain::createSwapChain()
{
    vk::PhysicalDevice physicalDevice = VulkanContext::Get().getPhysicalDevice();
    vk::SurfaceKHR surface = VulkanContext::Get().getSurface();
    vk::Device logicalDevice = VulkanContext::Get().getLogicalDevice();

    auto swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(physicalDevice, surface);

    vk::SurfaceFormatKHR surfaceFormat = swapChainSupportDetails.chooseSurfaceFormat();
    vk::PresentModeKHR presentMode = swapChainSupportDetails.choosePresentMode();
    vk::Extent2D extent = swapChainSupportDetails.chooseExtent();
    vk::SurfaceCapabilitiesKHR capabilities = swapChainSupportDetails.capabilities();
    std::uint32_t imageCount = swapChainSupportDetails.chooseImageCount();

    vk::SwapchainCreateInfoKHR swapChainCreateInfo = {
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1, // this value is always 1 unless we are developing stereoscopic 3D application
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        // opaque, because we don't need blending with other windows
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        // specify between which queue families ownership will be transfered
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        // Exclusive mode offers better perfomance, but requires explicit ownership transfer when using multiple queue families
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    m_swapChain = logicalDevice.createSwapchainKHR(swapChainCreateInfo);

    m_swapChainImages = logicalDevice.getSwapchainImagesKHR(m_swapChain);
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

void VulkanSwapchain::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (std::size_t i = 0; i < m_swapChainImages.size(); ++i)
    {
        const vk::ComponentMapping componentMapping = {
            .r = vk::ComponentSwizzle::eIdentity,
            .g = vk::ComponentSwizzle::eIdentity,
            .b = vk::ComponentSwizzle::eIdentity,
            .a = vk::ComponentSwizzle::eIdentity
        };

        const vk::ImageSubresourceRange imageSubresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        const vk::ImageViewCreateInfo createInfo = {
            .sType = vk::StructureType::eImageViewCreateInfo,
            .pNext = nullptr,
            .flags = {},
            .image = m_swapChainImages[i],
            .viewType = vk::ImageViewType::e2D,
            .format = m_swapChainImageFormat,
            .components = componentMapping,
            .subresourceRange = imageSubresourceRange
        };

        m_swapChainImageViews[i] = VulkanContext::Get().getLogicalDevice().createImageView(createInfo);
    }
}
