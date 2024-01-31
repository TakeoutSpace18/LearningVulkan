#include "VulkanVertexBuffer.h"

#include <vk_mem_alloc.h>

#include "VulkanContext.h"

// --------------- VulkanVertex ---------------

vk::VertexInputBindingDescription VulkanVertex::GetBindingDescription()
{
    return {
        .binding = 0,
        .stride = sizeof(VulkanVertex),
        .inputRate = vk::VertexInputRate::eVertex
    };
}

std::array<vk::VertexInputAttributeDescription, 2> VulkanVertex::GetAttributeDescriptions()
{
    std::array<vk::VertexInputAttributeDescription, 2> descriptions;

    descriptions[0] = {
        .location = 0,
        .binding = 0,
        .format = vk::Format::eR32G32Sfloat, // it's ok that color format is used here
        .offset = offsetof(VulkanVertex, position)
    };

    descriptions[1] = {
        .location = 1,
        .binding = 0,
        .format = vk::Format::eR32G32B32Sfloat,
        .offset = offsetof(VulkanVertex, color)
    };

    return descriptions;
}

// ------------ VulkanVertexBuffer ------------

VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<VulkanVertex>& vertices)
{
    create(vertices);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    cleanup();
}

vk::Buffer VulkanVertexBuffer::getHandle() const
{
    return m_buffer;
}

std::size_t VulkanVertexBuffer::getVertexCount() const
{
    return m_vertexCount;
}

void VulkanVertexBuffer::create(const std::vector<VulkanVertex>& vertices)
{
    m_vertexCount = vertices.size();
    vk::DeviceSize bufferSize = sizeof(vertices.front()) * vertices.size();

    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = vk::BufferCreateFlags(),
        .size = bufferSize,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0, // this is blank because buffer is not shared between queues
        .pQueueFamilyIndices = nullptr
    };

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationInfo allocationInfo;
    VkResult result = vmaCreateBuffer(VulkanContext::GetVmaAllocator(),
                                      (VkBufferCreateInfo *) &bufferCreateInfo,
                                      &allocationCreateInfo,
                                      (VkBuffer *) &m_buffer,
                                      &m_allocation,
                                      &allocationInfo);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("vmaCreateBuffer failed!");
    }
    std::memcpy(allocationInfo.pMappedData, vertices.data(), bufferSize);
}

void VulkanVertexBuffer::cleanup() noexcept
{
    VulkanContext::GetLogicalDevice().waitIdle();
    vmaDestroyBuffer(VulkanContext::GetVmaAllocator(), m_buffer, m_allocation);
}

std::uint32_t VulkanVertexBuffer::findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memoryProperties = VulkanContext::GetPhysicalDevice().getMemoryProperties();
    for (std::uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
