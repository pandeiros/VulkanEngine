/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Buffer.h"
#include "Core.h"

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
}

void Buffer::Allocate(Device& device, const VkMemoryPropertyFlags requiredProperties)
{
    VkMemoryRequirements requirements = GetMemoryRequirements(device.GetVkDevice());
    uint32_t memoryTypeIndex = device.GetPhysicalDevice()->GetMemoryTypeIndex(&requirements,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    memory.Allocate(device.GetVkDevice(), requirements.size, memoryTypeIndex);

    device.BindBufferMemory(buffer, memory.GetVkMemory(), requirements.size);
}

void Buffer::Copy(VkDevice device, void* sourceData, uint32_t offset, uint32_t size)
{
    void* destinationData = memory.MapMemory(device, offset, GetMemoryRequirements(device).size, 0);
    memcpy(destinationData, sourceData, size);
    memory.UnmapMemory(device);
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
