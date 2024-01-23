#ifndef VULKANRENDERPIPELINE_H
#define VULKANRENDERPIPELINE_H
#include <string>
#include <vector>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


class VulkanRenderPipeline {
public:

    void init();
    void destroy() noexcept;

private:
    std::vector<char> readFile(const std::string& filename);

    void createPipeline();
    void createCommandPool();
    void createCommandBuffer();

    void recordCommandBuffer(vk::CommandBuffer commandBuffer, std::uint32_t imageIndex);

    vk::ShaderModule createShaderModule(const std::vector<char>& code);

private:
    vk::PipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    vk::Pipeline m_graphicsPipeline = VK_NULL_HANDLE;

    vk::CommandPool m_commandPool;
    vk::CommandBuffer  m_commandBuffer;
};



#endif //VULKANRENDERPIPELINE_H
