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

void Renderer::CreateDescriptorSetLayout(VkDevice device)
{

    //std::vector<VkAttachmentDescription> attachments;
    //attachments.push_back(
    //{
    //    0,
    //    surfaceFormat.format,
    //    VK_SAMPLE_COUNT_1_BIT,
    //    VK_ATTACHMENT_LOAD_OP_CLEAR,
    //    VK_ATTACHMENT_STORE_OP_STORE,
    //    VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    //    VK_ATTACHMENT_STORE_OP_DONT_CARE,
    //    VK_IMAGE_LAYOUT_UNDEFINED,
    //    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    //});
    //attachments.push_back(
    //{
    //    0,
    //    depthStencilImage.GetVkFormat(),
    //    VK_SAMPLE_COUNT_1_BIT,
    //    VK_ATTACHMENT_LOAD_OP_CLEAR,
    //    VK_ATTACHMENT_STORE_OP_DONT_CARE,
    //    VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    //    VK_ATTACHMENT_STORE_OP_STORE,
    //    VK_IMAGE_LAYOUT_UNDEFINED,
    //    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    //});

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

    // Uniform buffer.
    layoutBindings.push_back(
    {
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        1,
        VK_SHADER_STAGE_VERTEX_BIT,
        nullptr
    });

    // Texture.
    if (bTextureEnabled)
    {
        layoutBindings.push_back(
        {
            1,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            1,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            nullptr
        });
    }

    uint32_t bindingCount = bTextureEnabled ? 2 : 1;
    descriptorSetLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        1,
        bindingCount,
        layoutBindings.data()
    };

    descriptorSetLayouts.resize(VULKAN_DESCRIPTOR_SETS_COUNT);
    VK_VERIFY(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayouts.data()));
}

void Renderer::CreatePipelineLayout(VkDevice device)
{
    pipelineLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)VULKAN_DESCRIPTOR_SETS_COUNT,
        descriptorSetLayouts.data(),
        0,
        nullptr
    };

    VK_VERIFY(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}

void Renderer::InitShaders(const char* vertexShaderText, const char* fragmentShaderText)
{
    if (!(vertexShaderText || fragmentShaderText))
    {
        VK_LOG(LogRenderer, Error, "Invalid shaders!");
        return;
    }

    ShaderTools::InitGLSLang();

    VkShaderModuleCreateInfo moduleCreateInfo;

    //if (vertexShaderText)
    //{
    //    std::vector<unsigned int> vtx_spv;

    //    pipelineShaderStageCreateInfo.push_back(
    //    {
    //        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    //        nullptr,
    //        0,
    //        VK_SHADER_STAGE_VERTEX_BIT,
    //        nullptr,
    //        "main",
    //        nullptr
    //    });

    //    retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
    //    assert(retVal);

    //    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    //    moduleCreateInfo.pNext = NULL;
    //    moduleCreateInfo.flags = 0;
    //    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
    //    moduleCreateInfo.pCode = vtx_spv.data();
    //    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[0].module);
    //    assert(res == VK_SUCCESS);
    //}

    ShaderTools::FinalizeGLSLang();
}

Buffer& Renderer::GetUniformBuffer()
{
    return uniformBuffer;
}
