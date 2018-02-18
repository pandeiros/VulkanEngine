/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Device.h"
#include "Memory.h"
#include "Utils/Math.h"

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

    void Allocate(Device* device, const VkMemoryPropertyFlags requiredProperties);
    void* Copy(VkDevice device, void* sourceData, uint32_t offset, uint32_t size);// , Transform transform = Transform());

    Memory& GetMemory();
    VkMemoryRequirements GetMemoryRequirements(VkDevice device);

    void UpdateDescriptorInfo(VkDeviceSize offset, VkDeviceSize range);

    VkBuffer* GetVkBufferPtr();
    VkDescriptorBufferInfo* GetDescriptorInfo();

    void SetStride(const uint32_t stride);
    uint32_t GetStride() const;

protected:
    VkBuffer buffer = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferCreateInfo = {};
    VkDescriptorBufferInfo descriptorInfo = {};

    Memory memory;

    uint32_t stride = 0;
    size_t usedSize = 0;
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

/**
 * @class SparseImage
 */
class VertexBuffer : public Buffer
{
public:
    uint32_t GetVertexCount();

private:

};

VULKAN_NS_END