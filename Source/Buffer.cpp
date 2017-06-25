/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Buffer.h"

VULKAN_NS_USING;

void Buffer::CreateExclusive(VkDevice device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage)
{
    bufferCreateInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        flags,
        size,
        usage,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        nullptr
    };

    DebugTools::Verify(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));
}

void Buffer::CreateConcurrent(VkDevice device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, std::vector<uint32_t> queueFamilyIndices)
{
    bufferCreateInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        flags,
        size,
        usage,
        VK_SHARING_MODE_CONCURRENT,
        (uint32_t)queueFamilyIndices.size(),
        queueFamilyIndices.data()
    };

    DebugTools::Verify(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));
}

void Buffer::Destroy(VkDevice device)
{
    vkDestroyBuffer(device, buffer, nullptr);
}

VkMemoryRequirements Buffer::GetMemoryRequirements(VkDevice device)
{
    VkMemoryRequirements requirements;

    if (buffer)
    {
        vkGetBufferMemoryRequirements(device, buffer, &requirements);
    }

    return requirements;
}
