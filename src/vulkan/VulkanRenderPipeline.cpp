#include "VulkanRenderPipeline.h"

#include <fstream>
#include <ios>

#include "VulkanContext.h"

void VulkanRenderPipeline::init()
{
    vk::ShaderModule vertexShaderModule = createShaderModule(readFile("shaders/triangle.vert"));
    vk::ShaderModule fragmentShaderModule = createShaderModule(readFile("shaders/triangle.frag"));

    vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {
        .sType = vk::StructureType::ePipelineShaderStageCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineShaderStageCreateFlags(),
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = vertexShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {
        .sType = vk::StructureType::ePipelineShaderStageCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineShaderStageCreateFlags(),
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = fragmentShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        vertexShaderStageCreateInfo,
        fragmentShaderStageCreateInfo
    };

    std::vector dynamicStates = {
        vk::DynamicState::eScissor,
        vk::DynamicState::eViewport
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo = {
        .sType = vk::StructureType::ePipelineDynamicStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineDynamicStateCreateFlags(),
        .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    // vertices are hardcoded in shaders, so this is blank for now
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = vk::StructureType::ePipelineVertexInputStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineVertexInputStateCreateFlags(),
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
        .sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineInputAssemblyStateCreateFlags(),
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = VK_FALSE
    };

    const vk::Extent2D swapchainExtent = VulkanContext::Get().getSwapchain().getExtent();

    vk::Viewport viewport = {
        .x = 0,
        .y = 0,
        .width = static_cast<float>(swapchainExtent.width),
        .height = static_cast<float>(swapchainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    vk::Rect2D scissor = {
        .offset = {0, 0},
        .extent = swapchainExtent
    };

    vk::PipelineViewportStateCreateInfo viewportInfo = {
        .sType = vk::StructureType::ePipelineViewportStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineViewportStateCreateFlags(),
        .viewportCount = 1,
        .pViewports = nullptr, // not specifying it here because of dynamic state
        .scissorCount = 1,
        .pScissors = nullptr
    };

    vk::PipelineRasterizationStateCreateInfo rasterizationInfo = {
        .sType = vk::StructureType::ePipelineRasterizationStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineRasterizationStateCreateFlags(),
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisamplingInfo = {
        .sType = vk::StructureType::ePipelineMultisampleStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineMultisampleStateCreateFlags(),
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendInfo = {
        .sType = vk::StructureType::ePipelineColorBlendStateCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineColorBlendStateCreateFlags(),
        .logicOpEnable = VK_FALSE,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &pipelineColorBlendAttachmentState,
        .blendConstants = vk::ArrayWrapper1D<float, 4>{}
    };

    vk::PipelineLayoutCreateInfo layoutInfo = {
        .sType = vk::StructureType::ePipelineLayoutCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineLayoutCreateFlags(),
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    m_pipelineLayout = VulkanContext::GetLogicalDevice().createPipelineLayout(layoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = vk::StructureType::eGraphicsPipelineCreateInfo,
        .pNext = nullptr,
        .flags = vk::PipelineCreateFlags(),
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState = nullptr,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisamplingInfo,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = m_pipelineLayout,
        .renderPass = VK_NULL_HANDLE, // render pass is not required cause we're using dynamic rendering
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    m_graphicsPipeline = VulkanContext::GetLogicalDevice().createGraphicsPipeline(VK_NULL_HANDLE, pipelineCreateInfo).value;

    VulkanContext::GetLogicalDevice().destroyShaderModule(vertexShaderModule);
    VulkanContext::GetLogicalDevice().destroyShaderModule(fragmentShaderModule);
}

void VulkanRenderPipeline::destroy() noexcept
{
    VulkanContext::GetLogicalDevice().destroyPipeline(m_graphicsPipeline);
    VulkanContext::GetLogicalDevice().destroyPipelineLayout(m_pipelineLayout);
}

std::vector<char> VulkanRenderPipeline::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file " + filename);
    }

    std::size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

vk::ShaderModule VulkanRenderPipeline::createShaderModule(const std::vector<char>& code)
{
    const vk::ShaderModuleCreateInfo createInfo = {
        .sType = vk::StructureType::eShaderModuleCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const std::uint32_t *>(code.data())
    };

    return VulkanContext::GetLogicalDevice().createShaderModule(createInfo);
}
