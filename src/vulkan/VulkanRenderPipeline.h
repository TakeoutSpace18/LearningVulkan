#ifndef VULKANRENDERPIPELINE_H
#define VULKANRENDERPIPELINE_H
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>


class VulkanRenderPipeline {
public:

    void init();
    void destroy() noexcept;

    void drawFrame();

private:
    std::vector<char> readFile(const std::string& filename);

    void createPipeline();
    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();

    void recordCommandBuffer(vk::CommandBuffer commandBuffer, std::uint32_t imageIndex);

    vk::ShaderModule createShaderModule(const std::vector<char>& code);

private:
    vk::PipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    vk::Pipeline m_graphicsPipeline = VK_NULL_HANDLE;

    vk::CommandPool m_commandPool;
    vk::CommandBuffer  m_commandBuffer;

    // syncronization
    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinishedSemaphore;
    vk::Fence m_inFlightFence;
};



#endif //VULKANRENDERPIPELINE_H
