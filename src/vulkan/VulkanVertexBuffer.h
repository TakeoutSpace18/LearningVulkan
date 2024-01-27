#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <utility/Utility.h>

struct VulkanVertex
{
public:
    glm::vec2 position;
    glm::vec3 color;

    static vk::VertexInputBindingDescription GetBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(const std::vector<VulkanVertex>& vertices);
    ~VulkanVertexBuffer() noexcept;

    NODISCARD vk::Buffer getHandle() const;
    NODISCARD std::size_t getVertexCount() const;

private:
    void create(const std::vector<VulkanVertex>& vertices);
    void cleanup() noexcept;

    std::uint32_t findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties);

private:
    vk::Buffer m_buffer;
    vk::DeviceMemory m_bufferMemory;

    std::size_t m_vertexCount;
};

#endif //VULKANVERTEXBUFFER_H
