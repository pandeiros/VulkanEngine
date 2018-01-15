/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Buffer.h"

#include <vector>

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

    void InitShaders(VkDevice device, const char* vertexShaderText, const char* fragmentShaderText);

    Buffer& GetUniformBuffer();
    Buffer& GetVertexBuffer();

    void AddVertexInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
    void AddVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

protected:
    Buffer uniformBuffer;
    Buffer vertexBuffer;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfo;

    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;

private:
    bool bTextureEnabled = false;
};

VULKAN_NS_END