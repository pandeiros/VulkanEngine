/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Buffer.h"

/**
 * @file Renderer.h
 */

VULKAN_NS_BEGIN

/**
 * Number of descriptor sets needs to match when allocation, creating pipeline layout
 * and creating descriptor set layout.
 */
#define VULKAN_DESCRIPTOR_SETS_COUNT 1

/*
 * @class Renderer
 */
class Renderer
{
public:
    /**
     * Default constructor.
     */
    Renderer() = default;

    /**
     * Default destructor.
     */
    ~Renderer() = default;

    void CreateDescriptorSetLayout(VkDevice device);
    void CreatePipelineLayout(VkDevice device);

    void InitShaders(const char* vertexShaderText, const char* fragmentShaderText);

    Buffer& GetUniformBuffer();

protected:
    Buffer uniformBuffer;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfo;

private:
    bool bTextureEnabled;
};

VULKAN_NS_END