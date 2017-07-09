/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"
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
    /**
     * Default constructor.
     */
    CommandPool() = default;

    /**
     * Default destructor.
     */
    ~CommandPool() = default;

    void Create(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex);
    void Destroy(VkDevice device);

    void Reset(VkDevice device, VkCommandPoolResetFlags flags);

    void AllocateCommandBuffer(VkDevice device, VkCommandBufferLevel level);

    void FreeCommandBuffer(VkDevice device, VkCommandPool commandPool);

    CommandBuffer& GetCommandBufferRef();

private:
    VkCommandPool commandPool = VK_NULL_HANDLE;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};

    CommandBuffer commandBuffer;
};

VULKAN_NS_END