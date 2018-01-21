/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Queue.h"
#include "Core.h"

VULKAN_NS_USING;

VkQueue& Queue::GetVkQueueRef()
{
    return queue;
}

void Queue::Submit(std::vector<VkSubmitInfo> submits, VkFence fence)
{
    VK_VERIFY(vkQueueSubmit(queue, (uint32_t)submits.size(), submits.data(), fence));
}

void Queue::Submit(const VkPipelineStageFlags* waitDstStageMask, const std::vector<VkSemaphore>& waitSemaphores,
    const std::vector<VkCommandBuffer>& commandBuffers, const std::vector<VkSemaphore>& signalSemaphores, VkFence fence)
{
    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        (uint32_t)waitSemaphores.size(),
        waitSemaphores.data(),
        waitDstStageMask,
        (uint32_t)commandBuffers.size(),
        commandBuffers.data(),
        (uint32_t)signalSemaphores.size(),
        signalSemaphores.data()
    };

    VK_VERIFY(vkQueueSubmit(queue, 1, &submitInfo, fence));
}

void Queue::BindSparse(const std::vector<VkBindSparseInfo> bindSparseInfo, VkFence fence)
{
    VK_VERIFY(vkQueueBindSparse(queue, (uint32_t)bindSparseInfo.size(), bindSparseInfo.data(), fence));
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

void Queue::WaitIdle()
{
    if (queue)
    {
        vkQueueWaitIdle(queue);
    }
}
