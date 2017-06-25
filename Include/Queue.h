/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"

/**
 * @file Queue.h
 */

VULKAN_NS_BEGIN

/**
 * @class Queue
 */
class Queue
{
public:
    /**
     * Default constructor.
     */
    Queue() = default;

    /**
     * Default destructor.
     */
    ~Queue() = default;

    VkQueue& GetVkQueueRef();

    void BindSparse(const std::vector<VkBindSparseInfo> bindSparseInfo, VkFence fence);

    void BindSparse(const std::vector<VkSemaphore> waitSemaphores, const std::vector<VkSemaphore> signalSemaphores,
        const std::vector<VkSparseBufferMemoryBindInfo> bufferBinds, const std::vector<VkSparseImageOpaqueMemoryBindInfo> imageOpaqueBinds,
        const std::vector<VkSparseImageMemoryBindInfo> imageBinds, VkFence fence);

private:
    VkQueue queue = VK_NULL_HANDLE;
};

VULKAN_NS_END