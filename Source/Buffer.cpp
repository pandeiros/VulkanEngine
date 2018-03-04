/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "Buffer.h"
#include "Core.h"
#include "Rendering//RenderComponent.h"

VULKAN_NS_USING;

void Buffer::CreateExclusive(VkDevice device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage)
{
    if (buffer)
    {
        Destroy(device);
    }

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

    VK_VERIFY(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));
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

    VK_VERIFY(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));
}

void Buffer::Destroy(VkDevice device)
{
    vkDestroyBuffer(device, buffer, nullptr);
    buffer = VK_NULL_HANDLE;

    memory.Free(device);
}

void Buffer::Allocate(Device* device, const VkMemoryPropertyFlags requiredProperties)
{
    VkMemoryRequirements requirements = GetMemoryRequirements(device->GetVkDevice());
    uint32_t memoryTypeIndex = device->GetPhysicalDevice()->GetMemoryTypeIndex(&requirements, requiredProperties);

    memory.Allocate(device->GetVkDevice(), requirements.size, memoryTypeIndex);

    device->BindBufferMemory(buffer, memory.GetVkMemory(), 0);
}

void* Buffer::Copy(VkDevice device, void* sourceData, uint32_t offset, uint32_t size)
{
    void* destinationData = memory.MapMemory(device, offset, size, 0);
    memcpy(destinationData, sourceData, size);

    usedSize += size;

    memory.UnmapMemory(device);

    return destinationData;
}

Memory& Buffer::GetMemory()
{
    return memory;
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

void Buffer::UpdateDescriptorInfo(VkDeviceSize offset, VkDeviceSize range)
{
    descriptorInfo = { buffer, offset, range };
}

VkBuffer* Buffer::GetVkBufferPtr()
{
    return &buffer;
}

VkDescriptorBufferInfo* Buffer::GetDescriptorInfo()
{
    return &descriptorInfo;
}

void Buffer::SetStride(const uint32_t stride)
{
    this->stride = stride;
}

uint32_t Buffer::GetStride() const
{
    return stride;
}

uint32_t VertexBuffer::GetVertexCount()
{
    return (uint32_t)usedSize / stride;
}
