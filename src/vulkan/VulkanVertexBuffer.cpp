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

std::pair<vk::Buffer, VmaAllocation> VulkanVertexBuffer::createStagingBuffer(vk::DeviceSize bufferSize, VmaAllocationInfo* allocationInfo)
{
    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = vk::BufferCreateFlags(),
        .size = bufferSize,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0, // this is blank because buffer is not shared between queues
        .pQueueFamilyIndices = nullptr
    };

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    vk::Buffer buffer;
    VmaAllocation allocation;
    VkResult result = vmaCreateBuffer(VulkanContext::GetDevice().getVmaAllocator(),
                                      (VkBufferCreateInfo *) &bufferCreateInfo,
                                      &allocationCreateInfo,
                                      (VkBuffer *) &buffer,
                                      &allocation,
                                      allocationInfo);

    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to create staging vertex buffer!");

    return std::make_pair(buffer, allocation);
}

std::pair<vk::Buffer, VmaAllocation> VulkanVertexBuffer::createDeviceLocalBuffer(vk::DeviceSize bufferSize, VmaAllocationInfo* allocationInfo)
{
    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .size = bufferSize,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        .sharingMode = vk::SharingMode::eExclusive,
    };

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocation allocation;
    vk::Buffer buffer;
    VkResult result = vmaCreateBuffer(VulkanContext::GetDevice().getVmaAllocator(),
                                      (VkBufferCreateInfo *) &bufferCreateInfo,
                                      &allocationCreateInfo,
                                      (VkBuffer *) &buffer,
                                      &allocation,
                                      allocationInfo);

    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to create device local vertex buffer!");

    return std::make_pair(buffer, allocation);
}

void VulkanVertexBuffer::copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
    vk::CommandBufferAllocateInfo allocateInfo = {
        .sType = vk::StructureType::eCommandBufferAllocateInfo,
        .commandPool = VulkanContext::GetDevice().getCommandPool(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };

    vk::CommandBuffer commandBuffer = VulkanContext::GetLogicalDevice().allocateCommandBuffers(allocateInfo).front();

    vk::CommandBufferBeginInfo beginInfo = {
        .sType = vk::StructureType::eCommandBufferBeginInfo,
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    commandBuffer.begin(beginInfo);

    const vk::BufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size
    };

    commandBuffer.copyBuffer(src, dst, copyRegion);

    commandBuffer.end();

    vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vk::FenceCreateInfo fenceCreateInfo = {
        .sType = vk::StructureType::eFenceCreateInfo
    };

    vk::Fence transferCompletedFence = VulkanContext::GetLogicalDevice().createFence(fenceCreateInfo);
    VulkanContext::GetDevice().getQueues().graphicsQueue.submit({submitInfo}, {transferCompletedFence});

    constexpr std::uint64_t timeout = std::numeric_limits<std::uint64_t>::max();
    vk::Result result = VulkanContext::GetLogicalDevice().waitForFences({transferCompletedFence}, VK_TRUE, timeout);
    ASSERT(result == vk::Result::eSuccess);

    VulkanContext::GetLogicalDevice().destroyFence(transferCompletedFence);
}

void VulkanVertexBuffer::create(const std::vector<VulkanVertex>& vertices)
{
    m_vertexCount = vertices.size();
    const vk::DeviceSize bufferSize = sizeof(vertices.front()) * vertices.size();

    VmaAllocationInfo stagingAllocInfo;
    auto [stagingBuffer, stagingAllocation] = createStagingBuffer(bufferSize, &stagingAllocInfo);
    std::memcpy(stagingAllocInfo.pMappedData, vertices.data(), bufferSize);

    std::tie(m_buffer, m_allocation) = createDeviceLocalBuffer(bufferSize, nullptr);
    copyBuffer(stagingBuffer, m_buffer, bufferSize);

    vmaDestroyBuffer(VulkanContext::GetDevice().getVmaAllocator(), stagingBuffer, stagingAllocation);
}

void VulkanVertexBuffer::cleanup() noexcept
{
    VulkanContext::GetLogicalDevice().waitIdle();
    vmaDestroyBuffer(VulkanContext::GetDevice().getVmaAllocator(), m_buffer, m_allocation);
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
