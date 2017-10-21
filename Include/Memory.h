/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file Memory.h
 */

VULKAN_NS_BEGIN

/**
 * @class Memory
 */
class Memory
{
public:
    /**
     * Default constructor.
     */
    Memory() = default;

    /**
     * Default destructor.
     */
    ~Memory() = default;

    void Allocate(VkDevice device, VkDeviceSize allocationSize, uint32_t memoryTypeIndex);

    void Free(VkDevice device);

    VkDeviceSize GetMemoryCommitment(VkDevice device);

    void* MapMemory(VkDevice device, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags);
    void UnmapMemory(VkDevice device);

    void FlushMappedMemoryRange(VkDevice device, VkDeviceSize offset, VkDeviceSize size);
    void InvalidateMappedMemoryRange(VkDevice device, VkDeviceSize offset, VkDeviceSize size);

    // #TODO This function is useless.
    VkSparseMemoryBind GetSparseMemoryBind(VkDeviceSize resourceOffset, VkDeviceSize size, VkDeviceSize memoryOffset, VkSparseMemoryBindFlags flags);

private:
    VkDeviceMemory memory = VK_NULL_HANDLE;

    VkMemoryAllocateInfo memoryAllocateInfo = {};
};

VULKAN_NS_END