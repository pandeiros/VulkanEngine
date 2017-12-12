/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "CommandPool.h"

VULKAN_NS_USING;

void CommandPool::Create(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex)
{
    commandPoolCreateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        flags,
        queueFamilyIndex
    };

    DebugTools::Verify(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
}

void CommandPool::Destroy(VkDevice device)
{
    // #TODO Check if buffers finished work.
    if (device && commandPool)
    {
        vkDestroyCommandPool(device, commandPool, nullptr);
    }
}

void CommandPool::Reset(VkDevice device, VkCommandPoolResetFlags flags)
{
    DebugTools::Verify(vkResetCommandPool(device, commandPool, flags));
}

void CommandPool::AllocateCommandBuffer(VkDevice device, VkCommandBufferLevel level)
{
    if (commandPool)
    {
        const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            nullptr,
            commandPool,
            level,
            1
        };

        DebugTools::Verify(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer.GetVkCommandBufferRef()));
    }
}

void CommandPool::FreeCommandBuffer(VkDevice device, VkCommandPool commandPool)
{
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer.GetVkCommandBufferRef());
}

CommandBuffer& CommandPool::GetCommandBufferRef()
{
    return commandBuffer;
}
