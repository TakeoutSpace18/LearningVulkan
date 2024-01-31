#ifndef VULKANRENDERPIPELINE_H
#define VULKANRENDERPIPELINE_H
#include <string>
#include <vector>
#include <utility/Utility.h>

#include <vulkan/vulkan.hpp>


class VulkanRenderPipeline {
public:

    void init();
    void destroy() noexcept;

    void drawFrame();

    // TODO: use different command pools for different purposes
    NODISCARD vk::CommandPool getCommandPool() const;

private:
    std::vector<char> readFile(const std::string& filename);

    void createPipeline();
    void createCommandBuffer();
    void createSyncObjects();

    void recordCommandBuffer(vk::CommandBuffer commandBuffer, std::uint32_t imageIndex);

    vk::ShaderModule createShaderModule(const std::vector<char>& code);

private:
    vk::PipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    vk::Pipeline m_graphicsPipeline = VK_NULL_HANDLE;

    vk::CommandBuffer  m_commandBuffer;

    // syncronization
    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinishedSemaphore;
    vk::Fence m_inFlightFence;
};



#endif //VULKANRENDERPIPELINE_H
