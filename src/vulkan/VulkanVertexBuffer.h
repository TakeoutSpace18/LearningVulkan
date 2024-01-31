#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include <vk_mem_alloc.h>
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

    // TODO: use single heap buffer for all staging data
    std::pair<vk::Buffer, VmaAllocation> createStagingBuffer(vk::DeviceSize bufferSize, VmaAllocationInfo* allocationInfo);
    std::pair<vk::Buffer, VmaAllocation> createDeviceLocalBuffer(vk::DeviceSize bufferSize, VmaAllocationInfo* allocationInfo);
    void copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

    std::uint32_t findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties);

private:
    vk::Buffer m_buffer;
    VmaAllocation  m_allocation;

    std::size_t m_vertexCount;
};

#endif //VULKANVERTEXBUFFER_H
