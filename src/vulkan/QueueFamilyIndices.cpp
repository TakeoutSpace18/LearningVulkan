#include "QueueFamilyIndices.h"

bool QueueFamilyIndices::isComplete() const
{
    return graphicsFamily.has_value() && presentFamily.has_value();
}

std::set<std::uint32_t> QueueFamilyIndices::getUniqueIndices() const
{
    return {
        graphicsFamily.value(),
        presentFamily.value()
    };
}

QueueFamilyIndices QueueFamilyIndices::FindQueueFamilies(const vk::PhysicalDevice& device,
                                                         const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices;
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
        vk::resultCheck(result, "Failed to check physical device present support!");

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        ++i;
    }
    return indices;
}
