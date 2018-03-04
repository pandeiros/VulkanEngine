/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "CommandPool.h"
#include "Core.h"

VULKAN_NS_USING;

void CommandPool::Create(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex)
{
    commandPoolCreateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        flags,
        queueFamilyIndex
    };

    VK_VERIFY(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
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
    VK_VERIFY(vkResetCommandPool(device, commandPool, flags));
}

void CommandPool::AllocateCommandBuffers(VkDevice device, VkCommandBufferLevel level, uint32_t commandBufferCount)
{
    if (commandPool)
    {
        const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            nullptr,
            commandPool,
            level,
            commandBufferCount
        };

        std::vector<VkCommandBuffer> vkCommandBuffers(commandBufferCount);

        VK_VERIFY(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, vkCommandBuffers.data()));

        for (VkCommandBuffer vkCommandBuffer : vkCommandBuffers)
        {
            commandBuffers.push_back(CommandBuffer(vkCommandBuffer));
        }
    }
}

void CommandPool::FreeCommandBuffers(VkDevice device, VkCommandPool commandPool)
{
    for (CommandBuffer& commandBuffer : commandBuffers)
    {
        vkFreeCommandBuffers(device, commandPool, 1, commandBuffer.GetVkCommandBufferPtr());
    }
}

CommandBuffer& CommandPool::GetCommandBufferRef(uint32_t commandBufferIndex)
{
    VK_ASSERT(commandBufferIndex < commandBuffers.size(), "Command buffer index is out of scope.");

    return commandBuffers[commandBufferIndex];
}

std::vector<VkCommandBuffer> CommandPool::GetVkCommandBuffers(std::vector<uint32_t> commandBufferIndexes)
{
    std::vector<VkCommandBuffer> vkCommandBuffers;

    for (uint32_t index : commandBufferIndexes)
    {
        vkCommandBuffers.push_back(commandBuffers[index].GetVkCommandBuffer());
    }

    return vkCommandBuffers;
}

std::vector<CommandBuffer>& CommandPool::GetCommandBuffers()
{
    return commandBuffers;
}
