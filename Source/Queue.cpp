/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Queue.h"

VULKAN_NS_USING;

VkQueue& Queue::GetVkQueueRef()
{
    return queue;
}

void Queue::BindSparse(const std::vector<VkBindSparseInfo> bindSparseInfo, VkFence fence)
{
    DebugTools::Verify(vkQueueBindSparse(queue, (uint32_t)bindSparseInfo.size(), bindSparseInfo.data(), fence));
}

void Queue::BindSparse(const std::vector<VkSemaphore> waitSemaphores, const std::vector<VkSemaphore> signalSemaphores, const std::vector<VkSparseBufferMemoryBindInfo> bufferBinds, const std::vector<VkSparseImageOpaqueMemoryBindInfo> imageOpaqueBinds, const std::vector<VkSparseImageMemoryBindInfo> imageBinds, VkFence fence)
{
    BindSparse({
        {
            VK_STRUCTURE_TYPE_BIND_SPARSE_INFO,
            nullptr,
            (uint32_t)waitSemaphores.size(),
            waitSemaphores.data(),
            (uint32_t)bufferBinds.size(),
            bufferBinds.data(),
            (uint32_t)imageOpaqueBinds.size(),
            imageOpaqueBinds.data(),
            (uint32_t)imageBinds.size(),
            imageBinds.data(),
            (uint32_t)signalSemaphores.size(),
            signalSemaphores.data(),
        }
    },
    fence);
}