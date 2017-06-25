/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"

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

    VkMemoryRequirements GetMemoryRequirements(VkDevice device);

protected:
    VkBuffer buffer = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferCreateInfo = {};
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