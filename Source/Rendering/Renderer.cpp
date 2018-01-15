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

void Renderer::InitShaders(VkDevice device, const char* vertexShaderText, const char* fragmentShaderText)
{
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

        VK_VERIFY(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[0].module));
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

        VK_VERIFY(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[1].module));
    }

    ShaderTools::FinalizeGLSLang();
}

Buffer& Renderer::GetUniformBuffer()
{
    return uniformBuffer;
}

Buffer& Renderer::GetVertexBuffer()
{
    return vertexBuffer;
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