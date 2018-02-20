/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "CommandPool.h"
#include "Buffer.h"
#include "ShaderTools.h"
#include "RenderComponent.h"

#include <vector>
#include <map>

/**
 * @file Renderer.h
 */

VULKAN_NS_BEGIN

/**
 * Number of descriptor sets needs to match during allocation, creating pipeline layout
 * and creating descriptor set layout.
 */
#define VULKAN_DESCRIPTOR_SETS_COUNT 1

#ifdef __ANDROID__
#define VULKAN_VIEWPORT_COUNT 1
#elif defined(VULKAN_VR_MODE_VIEWPORTS)
#define VULKAN_VIEWPORT_COUNT 2
#else
#define VULKAN_VIEWPORT_COUNT 1
#endif

/**
 * Should be the same during pipeline creation and during any call that change
 * their values.
 */
#define VULKAN_SCISSOR_COUNT VULKAN_VIEWPORT_COUNT

/**
 * Regardless of having multi viewports feature enabled or not, we create a buffer for each camera.
 */
#ifdef VULKAN_VR_MODE
#define VULKAN_COMMAND_BUFFER_COUNT 2
#else
#define VULKAN_COMMAND_BUFFER_COUNT 1
#endif

class Engine;

/*
 * @class Renderer
 */
class Renderer : public VulkanClass
{
public:
    Renderer(std::shared_ptr<Device> device, Engine* engine);
    ~Renderer();

    virtual void Init() override;
    virtual void Tick(float deltaTime) override;

    //////////////////////////////////////////////////////////////////////////
    // Settings
    //////////////////////////////////////////////////////////////////////////

private:
    Engine* engine;

    bool bTextureEnabled = false;
    bool bIncludeVertexInput = true;

    //////////////////////////////////////////////////////////////////////////
    // CommandPool & Buffers
    //////////////////////////////////////////////////////////////////////////

public:
    Buffer& GetUniformBuffer(uint32_t index);
    VertexBuffer& GetVertexBuffer();

    void BindVertexBuffers(VkCommandBuffer commandBuffer, std::vector<VkDeviceSize> offsets);

protected:
    CommandPool commandPool;
    std::vector<Buffer> uniformBuffers;
    VertexBuffer vertexBuffer;

    //////////////////////////////////////////////////////////////////////////
    // Descriptor pool/sets
    //////////////////////////////////////////////////////////////////////////

public:
    void CreateDescriptorSetLayout();
    void InitDescriptorPool();
    void InitDescriptorSet();

    void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, std::vector<uint32_t> dynamicOffsets);

private:
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};

    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkWriteDescriptorSet> writeDescriptorSets;

    //////////////////////////////////////////////////////////////////////////
    // Pipeline
    //////////////////////////////////////////////////////////////////////////

public:
    void CreatePipelineLayout();

    void InitPipelineCache();
    void InitPipeline(VkExtent2D size, VkRenderPass renderPass);

    void BindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint);

    void AddVertexInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
    void AddVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

private:
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    VkPipelineTessellationStateCreateInfo pipelineTesselationStateCreateInfo = {};
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    VkPipelineRasterizationStateCreateInfo pipelineRasterizationCreateInfo = {};
    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    std::vector<VkPipelineColorBlendAttachmentState> pipelineColorBlendAttachmentState;

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    VkPipeline pipeline = VK_NULL_HANDLE;

    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;

    VkPolygonMode polygonMode = VK_POLYGON_MODE_LINE;
    VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    //////////////////////////////////////////////////////////////////////////
    // Shaders
    //////////////////////////////////////////////////////////////////////////

public:
    size_t CompileShader(const char* shaderText, VkShaderStageFlagBits shaderType);

    // #TODO Refactor to compile all necessary shaders when initializing.
    // Need to remember which shader was already compiled to avoid multiple compilations.
    // Should enable to call this function multiple times to compile new shaders.
    //void InitShaders(const char* vertexShaderText, const char* fragmentShaderText);

    void InitShaders(ShaderIndexData& shaderIndexData);

private:
    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfo;

    std::map<size_t, ShaderCache> shaderCacheData;

    bool bShouldEnableTessellation = false;

    //////////////////////////////////////////////////////////////////////////
    // Viewports/Scissors/Render area
    //////////////////////////////////////////////////////////////////////////

public:
    void CommandSetViewports(VkCommandBuffer commandBuffer);
    void CommandSetScissors(VkCommandBuffer commandBuffer, uint32_t index);

protected:
    VkRect2D GetRenderArea(uint32_t viewportIndex);
    std::vector<VkClearValue> GetClearValues();

    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;

    //////////////////////////////////////////////////////////////////////////
    // Render components
    //////////////////////////////////////////////////////////////////////////

public:
    RenderComponent* AddRenderComponent(VertexData vertexData, ShaderEntry shaderEntry);

private:
    std::vector<std::unique_ptr<RenderComponent>> renderComponents;

//    //////////////////////////////////////////////////////////////////////////
//    // Command pool
//    //////////////////////////////////////////////////////////////////////////
//
//private:
//    CommandPool commandPool;
};

VULKAN_NS_END