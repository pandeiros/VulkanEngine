/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "Memory.h"
#include "Core.h"

VULKAN_NS_USING;

void Memory::Allocate(VkDevice device, VkDeviceSize allocationSize, uint32_t memoryTypeIndex)
{
    memoryAllocateInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        allocationSize,
        memoryTypeIndex
    };

    VK_VERIFY(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory));
}

void Memory::Free(VkDevice device)
{
    if (device)
    {
        vkFreeMemory(device, memory, nullptr);
    }
}

VkDeviceMemory Memory::GetVkMemory() const
{
    return memory;
}

VkDeviceSize Memory::GetMemoryCommitment(VkDevice device)
{
    VkDeviceSize size;

    if (memory)
    {
        vkGetDeviceMemoryCommitment(device, memory, &size);
    }

    return size;
}

void* Memory::MapMemory(VkDevice device, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags)
{
    void* data = nullptr;

    if (memory)
    {
        VK_VERIFY(vkMapMemory(device, memory, offset, size, flags, &data));
    }

    return data;
}

void Memory::UnmapMemory(VkDevice device)
{
    vkUnmapMemory(device, memory);
}

void Memory::InvalidateMappedMemoryRange(VkDevice device, VkDeviceSize offset, VkDeviceSize size)
{
    VkMappedMemoryRange range = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        nullptr,
        memory,
        offset,
        size
    };

    VK_VERIFY(vkFlushMappedMemoryRanges(device, 1, &range));
}

void Memory::BindImageMemory(VkDevice device, VkImage image, VkDeviceSize memoryOffset)
{
    VK_VERIFY(vkBindImageMemory(device, image, memory, memoryOffset));
}

void Memory::FlushMappedMemoryRange(VkDevice device, VkDeviceSize offset, VkDeviceSize size)
{
    VkMappedMemoryRange range = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        nullptr,
        memory,
        offset,
        size
    };

    VK_VERIFY(vkInvalidateMappedMemoryRanges(device, 1, &range));
}
