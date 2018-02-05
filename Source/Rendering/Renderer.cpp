/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Rendering/Renderer.h"

#include "Core.h"
#include "Rendering/ShaderTools.h"

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogRenderer);

Renderer::Renderer(std::shared_ptr<Device> device, Engine* engine)
    : VulkanClass(device), engine(engine)
{
    bIncludeVertexInput = true;

    uniformBuffers.resize(VULKAN_DESCRIPTOR_SETS_COUNT);

    commandPool.Create(device->GetVkDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        device->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex());

    commandPool.AllocateCommandBuffers(device->GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VULKAN_COMMAND_BUFFER_COUNT);
}

Renderer::~Renderer()
{
    commandPool.Destroy(device->GetVkDevice());

    vkDestroyPipeline(device->GetVkDevice(), pipeline, nullptr);
    vkDestroyPipelineCache(device->GetVkDevice(), pipelineCache, nullptr);
    vkDestroyDescriptorPool(device->GetVkDevice(), descriptorPool, nullptr);

    vertexBuffer.Destroy(device->GetVkDevice());

    for (VkPipelineShaderStageCreateInfo& shaderStage : pipelineShaderStageCreateInfo)
    {
        vkDestroyShaderModule(device->GetVkDevice(), shaderStage.module, nullptr);
    }

    for (VkDescriptorSetLayout& descriptorSetLayout : descriptorSetLayouts)
    {
        vkDestroyDescriptorSetLayout(device->GetVkDevice(), descriptorSetLayout, nullptr);
    }

    vkDestroyPipelineLayout(device->GetVkDevice(), pipelineLayout, nullptr);

    for (Buffer& uniformBuffer : uniformBuffers)
    {
        uniformBuffer.Destroy(device->GetVkDevice());
    }
}

void Renderer::Tick(float deltaTime)
{
    VK_PERFORMANCE_SECTION("Renderer update");
}

void Renderer::Init()
{
    VK_PERFORMANCE_SECTION("Renderer init");

    for (uint32_t i = 0; i < VULKAN_DESCRIPTOR_SETS_COUNT; ++i)
    {
        uniformBuffers[i].CreateExclusive(device->GetVkDevice(), 0, 2 * 256, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT); // #TODO Use device properties instead of 256
        uniformBuffers[i].Allocate(device.get(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        uniformBuffers[i].UpdateDescriptorInfo(0, VK_WHOLE_SIZE);
    }

    CreateDescriptorSetLayout();
    CreatePipelineLayout();
    InitShaders(VULKAN_VERTEX_SHADER_TEXT, VULKAN_FRAGMENT_SHADER_TEXT);
}

Buffer& Renderer::GetUniformBuffer(uint32_t index)
{
    return uniformBuffers[index];
}

Buffer& Renderer::GetVertexBuffer()
{
    return vertexBuffer;
}

void Renderer::BindVertexBuffers(VkCommandBuffer commandBuffer, std::vector<VkDeviceSize> offsets)
{
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer.GetVkBufferPtr(), offsets.data());
}

void Renderer::CreateDescriptorSetLayout()
{
    VK_PERFORMANCE_SECTION("Create Descriptor Set Layout");

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

    // Uniform buffer.
    layoutBindings.push_back(
    {
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        1,
        VK_SHADER_STAGE_VERTEX_BIT,
        nullptr
    });

    //// Texture.
    //if (bTextureEnabled)
    //{
    //    layoutBindings.push_back(
    //    {
    //        1,
    //        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    //        1,
    //        VK_SHADER_STAGE_FRAGMENT_BIT,
    //        nullptr
    //    });
    //}

    descriptorSetLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)layoutBindings.size(),
        layoutBindings.data()
    };

    descriptorSetLayouts.resize(VULKAN_DESCRIPTOR_SETS_COUNT);
    VK_VERIFY(vkCreateDescriptorSetLayout(device->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayouts.data()));
}

void Renderer::InitDescriptorPool(VkDevice device)
{
    descriptorPoolSizes.push_back({
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        1
    });

    if (bTextureEnabled)
    {
        descriptorPoolSizes.push_back({
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            1
        });
    }

    descriptorPoolCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        nullptr,
        0,
        VULKAN_DESCRIPTOR_SETS_COUNT,
        (uint32_t)descriptorPoolSizes.size(),
        descriptorPoolSizes.data()
    };

    VK_VERIFY(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
}

void Renderer::InitDescriptorSet(VkDevice device)
{
    descriptorSetAllocateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        descriptorPool,
        VULKAN_DESCRIPTOR_SETS_COUNT,
        descriptorSetLayouts.data()
    };

    descriptorSets.resize(VULKAN_DESCRIPTOR_SETS_COUNT);
    VK_VERIFY(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data()));

    for (uint32_t i = 0; i < VULKAN_DESCRIPTOR_SETS_COUNT; ++i)
    {
        writeDescriptorSets.push_back({
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            descriptorSets[i],
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            nullptr,
            uniformBuffers[i].GetDescriptorInfo(),
            nullptr
        });
    }

    vkUpdateDescriptorSets(device, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
}

void Renderer::BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, std::vector<uint32_t> dynamicOffsets)
{
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipelineLayout, 0,
        VULKAN_DESCRIPTOR_SETS_COUNT, descriptorSets.data(), (uint32_t)dynamicOffsets.size(), dynamicOffsets.data());
}

void Renderer::CreatePipelineLayout()
{
    VK_PERFORMANCE_SECTION("Create Pipeline Layout");

    pipelineLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)descriptorSetLayouts.size(),
        descriptorSetLayouts.data(),
        0,
        nullptr
    };

    VK_VERIFY(vkCreatePipelineLayout(device->GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}

void Renderer::InitPipelineCache(VkDevice device)
{
    pipelineCacheCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr
    };

    VK_VERIFY(vkCreatePipelineCache(device, &pipelineCacheCreateInfo, NULL, &pipelineCache));
}

void Renderer::InitPipeline(VkDevice device, VkExtent2D size, VkRenderPass renderPass)
{
    VkDynamicState dynamicStates[VK_DYNAMIC_STATE_RANGE_SIZE];
    memset(dynamicStates, 0, sizeof(dynamicStates));

    dynamicStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        nullptr,
        0,
        0,
        dynamicStates
    };

    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    if (bIncludeVertexInput)
    {
        pipelineVertexInputStateCreateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            (uint32_t)vertexInputBindings.size(),
            vertexInputBindings.data(),
            (uint32_t)vertexInputAttributes.size(),
            vertexInputAttributes.data()
        };
    }

    pipelineInputAssemblyStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_FALSE
    };

    pipelineRasterizationCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE,
        VK_FALSE,
        VK_POLYGON_MODE_FILL,
        VK_CULL_MODE_BACK_BIT,
        VK_FRONT_FACE_CLOCKWISE,
        VK_FALSE,
        0,
        0,
        0,
        1.f
    };

    pipelineColorBlendAttachmentState.push_back({
        VK_FALSE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        0xf
    });

    pipelineColorBlendStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE,
        VK_LOGIC_OP_NO_OP,
        (uint32_t)pipelineColorBlendAttachmentState.size(),
        pipelineColorBlendAttachmentState.data(),
        { 1.f, 1.f, 1.f, 1.f }
    };

    pipelineViewportStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        nullptr,
        0,
        VULKAN_VIEWPORT_COUNT,
        nullptr,
        VULKAN_SCISSOR_COUNT,
        nullptr
    };

    viewports.push_back({
        0.f, 0.f,
        (float)size.width / VULKAN_VIEWPORT_COUNT, (float)size.height,
        0.f, 1.f
    });

    scissors.push_back({
        { 0, 0 },
        { size.width / VULKAN_COMMAND_BUFFER_COUNT, size.height }
    });

#ifdef VULKAN_VR_MODE
    scissors.push_back({
        { (int32_t)size.width / VULKAN_COMMAND_BUFFER_COUNT, 0 },
        { size.width / VULKAN_COMMAND_BUFFER_COUNT, size.height }
    });
#elif defined(VULKAN_VR_MODE_VIEWPORTS)
    viewports.push_back({
        (float)size.width / VULKAN_VIEWPORT_COUNT, 0.f,
        (float)size.width / VULKAN_VIEWPORT_COUNT, (float)size.height,
        0.f, 1.f
    });

    scissors.push_back({
        { (int32_t)size.width / VULKAN_VIEWPORT_COUNT, 0 },
        { size.width / VULKAN_VIEWPORT_COUNT, size.height }
    });
#endif

    dynamicStates[dynamicStateCreateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[dynamicStateCreateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

    VkStencilOpState stencilOpState = {
        VK_STENCIL_OP_KEEP,
        VK_STENCIL_OP_KEEP,
        VK_STENCIL_OP_KEEP,
        VK_COMPARE_OP_ALWAYS,
        0,
        0,
        0
    };

    pipelineDepthStencilStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_TRUE,
        VK_TRUE,
        VK_COMPARE_OP_LESS_OR_EQUAL,
        VK_FALSE,
        VK_FALSE,
        stencilOpState,
        stencilOpState,
        0,
        0
    };

    pipelineMultisampleStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        nullptr,
        0,
        VULKAN_SAMPLE_COUNT,
        VK_FALSE,
        0.f,
        nullptr,
        VK_FALSE,
        VK_FALSE
    };

    graphicsPipelineCreateInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)pipelineShaderStageCreateInfo.size(),
        pipelineShaderStageCreateInfo.data(),
        &pipelineVertexInputStateCreateInfo,
        &pipelineInputAssemblyStateCreateInfo,
        nullptr,
        &pipelineViewportStateCreateInfo,
        &pipelineRasterizationCreateInfo,
        &pipelineMultisampleStateCreateInfo,
        &pipelineDepthStencilStateCreateInfo,
        &pipelineColorBlendStateCreateInfo,
        &dynamicStateCreateInfo,
        pipelineLayout,
        renderPass,
        0,
        VK_NULL_HANDLE,
        0
    };

    VK_VERIFY(vkCreateGraphicsPipelines(device, pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));
}

void Renderer::BindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint)
{
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void Renderer::AddVertexInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
    vertexInputBindings.push_back({
        binding, stride, inputRate
    });
}

void Renderer::AddVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
{
    vertexInputAttributes.push_back({
        location, binding, format, offset
    });
}

void Renderer::InitShaders(const char* vertexShaderText, const char* fragmentShaderText)
{
    VK_PERFORMANCE_SECTION("Initialize Shaders");

    if (!(vertexShaderText || fragmentShaderText))
    {
        VK_LOG(LogRenderer, Error, "Invalid shaders!");
        return;
    }

    ShaderTools::InitGLSLang();

    VkShaderModuleCreateInfo moduleCreateInfo;

    if (vertexShaderText)
    {
        std::vector<unsigned int> vertexSPIRV;

        pipelineShaderStageCreateInfo.push_back({
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            VK_NULL_HANDLE,
            "main",
            nullptr
            });


        VK_ASSERT(ShaderTools::glslToSPIRV(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderText, vertexSPIRV), "Cannot convert shader to SPIR-V.\n%s", vertexShaderText);

        moduleCreateInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            nullptr,
            0,
            vertexSPIRV.size() * sizeof(unsigned int),
            vertexSPIRV.data()
        };

        VK_VERIFY(vkCreateShaderModule(device->GetVkDevice(), &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[0].module));
    }

    if (fragmentShaderText)
    {
        std::vector<unsigned int> fragmentSPIRV;

        pipelineShaderStageCreateInfo.push_back({
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            VK_NULL_HANDLE,
            "main",
            nullptr
            });

        VK_ASSERT(ShaderTools::glslToSPIRV(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderText, fragmentSPIRV), "Cannot convert shader to SPIR-V.\n%s", fragmentShaderText);

        moduleCreateInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            nullptr,
            0,
            fragmentSPIRV.size() * sizeof(unsigned int),
            fragmentSPIRV.data()
        };

        VK_VERIFY(vkCreateShaderModule(device->GetVkDevice(), &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[1].module));
    }

    ShaderTools::FinalizeGLSLang();
}

void Renderer::CommandSetViewports(VkCommandBuffer commandBuffer)
{
    vkCmdSetViewport(commandBuffer, 0, (uint32_t)viewports.size(), viewports.data());
}

void Renderer::CommandSetScissors(VkCommandBuffer commandBuffer, uint32_t index)
{
    vkCmdSetScissor(commandBuffer, 0, 1, &scissors[index]);
}
