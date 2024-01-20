#ifndef QUEUEFAMILYINDICES_H
#define QUEUEFAMILYINDICES_H
#include <optional>
#include <cstdint>
#include <set>

#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const;
    [[nodiscard]] std::set<std::uint32_t> getUniqueIndices() const;

    static QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
};


#endif //QUEUEFAMILYINDICES_H
