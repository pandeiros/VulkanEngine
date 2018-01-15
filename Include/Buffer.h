/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Device.h"
#include "Memory.h"

/**
 * @file Buffer.h
 */

VULKAN_NS_BEGIN

/**
  * @class Buffer
  */
class Buffer
{
public:
    /**
      * Default constructor.
      */
    Buffer() = default;

    /**
      * Default destructor.
      */
    ~Buffer() = default;

    virtual void CreateExclusive(VkDevice device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage);
    virtual void CreateConcurrent(VkDevice device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, std::vector<uint32_t> queueFamilyIndices);

    void Destroy(VkDevice device);

    void Allocate(Device& device, const VkMemoryPropertyFlags requiredProperties);
    void Copy(VkDevice device, void* sourceData, uint32_t offset, uint32_t size);

    Memory& GetMemory();
    VkMemoryRequirements GetMemoryRequirements(VkDevice device);

    void UpdateDescriptorInfo(VkDeviceSize offset, VkDeviceSize range);

protected:
    VkBuffer buffer = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferCreateInfo = {};
    VkDescriptorBufferInfo descriptorInfo = {};

    Memory memory;
};

/**
 * @class SparseImage
 */
class SparseBuffer : public Buffer
{
public:
    /**
     * Default constructor.
     */
    SparseBuffer() = default;

    /**
     * Default destructor.
     */
    ~SparseBuffer() = default;

    // #TODO Add SparseBufferMemoryBind

private:

};

VULKAN_NS_END