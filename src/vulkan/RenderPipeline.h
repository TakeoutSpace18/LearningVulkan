#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H
#include <string>
#include <vector>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


class RenderPipeline {
public:

    void init();
    void destroy() noexcept;

private:
    std::vector<char> readFile(const std::string& filename);

    vk::ShaderModule createShaderModule(const std::vector<char>& code);

private:
    vk::PipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};



#endif //RENDERPIPELINE_H
