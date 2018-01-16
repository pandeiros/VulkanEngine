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
 * Number of descriptor sets needs to match during allocation, creating pipeline layout
 * and creating descriptor set layout.
 */
#define VULKAN_DESCRIPTOR_SETS_COUNT 1

/**
 * Should be the same during pipeline creation and during any call that change
 * their values. Also they should have same value.
 */
#define VULKAN_VIEWPORT_COUNT 1
#define VULKAN_SCISSOR_COUNT VULKAN_VIEWPORT_COUNT

/*
 * @class Renderer
 * #TODO Find an elegant way to store VkDevice
 */
class Renderer : public VulkanClass
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
    void InitDescriptorPool(VkDevice device);
    void InitDescriptorSet(VkDevice device);
    void InitPipelineCache(VkDevice device);
    void InitPipeline(VkDevice device, VkExtent2D size, VkRenderPass renderPass);

    void BindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint);
    void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint);
    void BindVertexBuffers(VkCommandBuffer commandBuffer, std::vector<VkDeviceSize> offsets);

    void CommandSetViewports(VkCommandBuffer commandBuffer);
    void CommandSetScissors(VkCommandBuffer commandBuffer);

    Buffer& GetUniformBuffer();
    Buffer& GetVertexBuffer();

    void AddVertexInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
    void AddVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

protected:
    Buffer uniformBuffer;
    Buffer vertexBuffer;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};

    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkWriteDescriptorSet> writeDescriptorSets;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfo;

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    VkPipelineRasterizationStateCreateInfo pipelineRasterizationCreateInfo = {};
    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    std::vector<VkPipelineColorBlendAttachmentState> pipelineColorBlendAttachmentState;

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    VkPipeline pipeline = VK_NULL_HANDLE;

    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;

private:
    VkDevice device;

    bool bTextureEnabled = false;
    bool bIncludeVertexInput = true;
};

VULKAN_NS_END