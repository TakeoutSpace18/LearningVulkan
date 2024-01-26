#ifndef VULKANAPP_H
#define VULKANAPP_H

#include <vulkan/vulkan.hpp>

#include <memory>

#include "VulkanDevice.h"
#include "VulkanRenderPipeline.h"
#include "VulkanSwapchain.h"
#include "utility/NonCopyable.h"
#include "utility/NonMovable.h"
#include "utility/Utility.h"

class VulkanContext : NonCopyable, NonMovable
{
public:

    ~VulkanContext();

    static void Initialize();
    static  VulkanContext& Get();

    NODISCARD static vk::Device GetLogicalDevice();
    NODISCARD static vk::PhysicalDevice GetPhysicalDevice();
    NODISCARD static vk::Instance GetVulkanInstance();
    NODISCARD static vk::SurfaceKHR GetSurface();

    NODISCARD static VulkanSwapchain& GetSwapchain();
    NODISCARD static VulkanDevice& GetDevice();


    static void DrawFrame();

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
    VulkanRenderPipeline m_renderPipeline;

    static std::unique_ptr<VulkanContext> s_instance;
};

#endif //VULKANAPP_H
