/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
#include "CommandBuffer.h"

/**
 * @file CommandPool.h
 */

VULKAN_NS_BEGIN

/**
 * @class CommandPool
 */
class CommandPool
{
public:
    CommandPool() = default;

    ~CommandPool() = default;

    void Create(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex);
    void Destroy(VkDevice device);

    void Reset(VkDevice device, VkCommandPoolResetFlags flags);

    void AllocateCommandBuffers(VkDevice device, VkCommandBufferLevel level, uint32_t commandBufferCount);

    void FreeCommandBuffers(VkDevice device, VkCommandPool commandPool);

    CommandBuffer& GetCommandBufferRef(uint32_t commandBufferIndex);

    std::vector<VkCommandBuffer> GetVkCommandBuffers(std::vector<uint32_t> commandBufferIndexes);
    std::vector<CommandBuffer>& GetCommandBuffers();

private:
    VkCommandPool commandPool = VK_NULL_HANDLE;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};

    std::vector<CommandBuffer> commandBuffers;
};

VULKAN_NS_END