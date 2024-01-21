#ifndef VULKANAPP_H
#define VULKANAPP_H

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <memory>

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "utility/NonCopyable.h"
#include "utility/NonMovable.h"
#include "utility/Utility.h"

class VulkanContext : NonCopyable, NonMovable
{
public:

    ~VulkanContext();

    static void Initialize();
    static const VulkanContext& Get();

    NODISCARD static vk::Device GetLogicalDevice();
    NODISCARD static vk::PhysicalDevice GetPhysicalDevice();
    NODISCARD static vk::Instance GetVulkanInstance();
    NODISCARD static vk::SurfaceKHR GetSurface();

    NODISCARD const VulkanSwapchain& getSwapchain() const;
    NODISCARD const VulkanDevice& getDevice() const;

private:
    VulkanContext() = default;

    void init();

    void cleanup();

    std::vector<const char*> getRequiredInstanceExtensions();

    void createInstance();

    static void LogSupportedInstanceExtensions();

private:
    vk::Instance m_instance = VK_NULL_HANDLE;
    vk::SurfaceKHR m_surface = VK_NULL_HANDLE;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;

    static std::unique_ptr<VulkanContext> s_instance;
};

#endif //VULKANAPP_H
