#ifndef VULKANQUEUEFAMILYINDICES_H
#define VULKANQUEUEFAMILYINDICES_H
#include <optional>
#include <cstdint>
#include <set>

#include <vulkan/vulkan.hpp>

struct VulkanQueueFamilyIndices {
public:
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

public:
    [[nodiscard]] bool isComplete() const;
    [[nodiscard]] std::set<std::uint32_t> getUniqueIndices() const;

    static VulkanQueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
};


#endif //VULKANQUEUEFAMILYINDICES_H
