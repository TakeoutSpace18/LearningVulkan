#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include <csignal>
#include <cstddef>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <utility/Utility.h>
#include <vulkan/vulkan_enums.hpp>


struct VulkanVertex
{
public:
    glm::vec2 position;
    glm::vec3 color;

    static vk::VertexInputBindingDescription GetBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions();
};


class VulkanBuffer
{
public:
    VulkanBuffer(const void *data, std::size_t size, vk::BufferUsageFlags usageFlags);
    ~VulkanBuffer() noexcept;

    NODISCARD vk::Buffer getHandle() const;

private:
    void create(const void *data, std::size_t size, vk::BufferUsageFlags usageFlags);
    void cleanup() noexcept;

    // TODO: use single heap buffer for all staging data
    std::pair<vk::Buffer, VmaAllocation> createStagingBuffer(
        vk::DeviceSize bufferSize,
        VmaAllocationInfo* allocationInfo);

    std::pair<vk::Buffer, VmaAllocation> createDeviceLocalBuffer(
        vk::DeviceSize bufferSize,
        vk::BufferUsageFlags usageFlags,
        VmaAllocationInfo* allocationInfo);

    void copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

    std::uint32_t findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties);

private:
    vk::Buffer m_buffer;
    VmaAllocation  m_allocation;
};


class VulkanVertexBuffer : public VulkanBuffer
{
public:
    VulkanVertexBuffer(const std::vector<VulkanVertex>& vertices);
    NODISCARD std::size_t getVertexCount() const { return m_vertexCount; }

private:
    std::size_t m_vertexCount;
};

class VulkanIndexBuffer : public VulkanBuffer
{
public:
    using IndexType = uint16_t;

    VulkanIndexBuffer(const std::vector<IndexType> indices);
    NODISCARD std::size_t getIndexCount() const { return m_indexCount; }
    NODISCARD vk::IndexType getIndexType() const { return vk::IndexType::eUint16; }

private:
    std::size_t m_indexCount;
};

#endif //VULKANVERTEXBUFFER_H
