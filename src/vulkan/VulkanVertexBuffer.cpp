#include "VulkanVertexBuffer.h"

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

    vk::BufferCreateInfo createInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = vk::BufferCreateFlags(),
        .size = bufferSize,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0, // this is blank because buffer is not shared between queues
        .pQueueFamilyIndices = nullptr
    };

    m_buffer = VulkanContext::GetLogicalDevice().createBuffer(createInfo);

    vk::MemoryRequirements memoryRequirements = VulkanContext::GetLogicalDevice().getBufferMemoryRequirements(m_buffer);

    vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = bufferSize,
        .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible |
                                          // to ensure that buffer always matches allocated memory
                                          vk::MemoryPropertyFlagBits::eHostCoherent
        )
    };

    m_bufferMemory = VulkanContext::GetLogicalDevice().allocateMemory(memoryAllocateInfo);
    VulkanContext::GetLogicalDevice().bindBufferMemory(m_buffer, m_bufferMemory, 0);

    void* data = VulkanContext::GetLogicalDevice().mapMemory(m_bufferMemory, 0, bufferSize, vk::MemoryMapFlags());
    std::memcpy(data, vertices.data(), bufferSize);
    VulkanContext::GetLogicalDevice().unmapMemory(m_bufferMemory);
}

void VulkanVertexBuffer::cleanup() noexcept
{
    VulkanContext::GetLogicalDevice().destroyBuffer(m_buffer);
    VulkanContext::GetLogicalDevice().freeMemory(m_bufferMemory);
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
