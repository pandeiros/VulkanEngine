/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Memory.h"

VULKAN_NS_USING;

void Memory::Allocate(VkDevice device, VkDeviceSize allocationSize, uint32_t memoryTypeIndex)
{
    memoryAllocateInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        allocationSize,
        memoryTypeIndex
    };

    DebugTools::Verify(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory));
}

void Memory::Free(VkDevice device)
{
    vkFreeMemory(device, memory, nullptr);
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
        DebugTools::Verify(vkMapMemory(device, memory, offset, size, flags, &data));
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

    DebugTools::Verify(vkFlushMappedMemoryRanges(device, 1, &range));
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

    DebugTools::Verify(vkInvalidateMappedMemoryRanges(device, 1, &range));
}

VkSparseMemoryBind Memory::GetSparseMemoryBind(VkDeviceSize resourceOffset, VkDeviceSize size, VkDeviceSize memoryOffset, VkSparseMemoryBindFlags flags)
{
    return { resourceOffset, size, memory, memoryOffset, flags };
}
