#include "VulkanQueueFamilyIndices.h"

bool VulkanQueueFamilyIndices::isComplete() const
{
    return graphicsFamily.has_value() && presentFamily.has_value();
}

std::set<std::uint32_t> VulkanQueueFamilyIndices::getUniqueIndices() const
{
    return {
        graphicsFamily.value(),
        presentFamily.value()
    };
}

VulkanQueueFamilyIndices VulkanQueueFamilyIndices::FindQueueFamilies(const vk::PhysicalDevice& device,
                                                         const vk::SurfaceKHR& surface)
{
    VulkanQueueFamilyIndices indices;
    std::uint32_t i = 0;
    for (const auto family : device.getQueueFamilyProperties())
    {
        if (indices.isComplete()) break;

        if (family.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        vk::Bool32 presentSupport = false;
        const vk::Result result = device.getSurfaceSupportKHR(i, surface, &presentSupport);
        vk::detail::resultCheck(result, "Failed to check physical device present support!");

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        ++i;
    }
    return indices;
}
