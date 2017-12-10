/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Device.h"

VULKAN_NS_USING;

void Device::Create(PhysicalDevice& physicalDevice, std::vector<const char*> deviceExtensions, VkPhysicalDeviceFeatures requiredFeatures)
{
    cachedPhysicalDevice = &physicalDevice;
    cachedDeviceExtensions = deviceExtensions;
    cachedRequiredFeatures = requiredFeatures;

    CreateInternal();
}

void Device::Reset()
{
    Destroy();
    CreateInternal();

    cachedPhysicalDevice->SetDirty(false);
}

void Device::Destroy()
{
    if (device)
    {
        vkDestroyDevice(device, nullptr);
    }

    device = VK_NULL_HANDLE;
}

void Device::CheckPhysicalDeviceDirty()
{
    if (cachedPhysicalDevice && cachedPhysicalDevice->IsDirty())
    {
        Reset();
    }
}

VkDevice Device::GetVkDevice()
{
    return device;
}

Queue& Device::GetQueueRef()
{
    return queue;
}

PhysicalDevice* Device::GetPhysicalDevice()
{
    return cachedPhysicalDevice;
}

void Device::FlushMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges)
{
    DebugTools::Verify(vkFlushMappedMemoryRanges(device, (uint32_t)memoryRanges.size(), memoryRanges.data()));
}

void Device::InvalidateMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges)
{
    DebugTools::Verify(vkInvalidateMappedMemoryRanges(device, (uint32_t)memoryRanges.size(), memoryRanges.data()));
}

uint32_t Device::GetMemoryTypeForImage(const VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags preferredFlags, VkMemoryPropertyFlags requiredFlags)
{
    uint32_t selectedType = ~0u;
    VkPhysicalDeviceMemoryProperties deviceProperties = {};

    if (cachedPhysicalDevice)
    {
        deviceProperties = cachedPhysicalDevice->GetPhysicalDeviceMemoryProperties();
    }
    else
    {
        return selectedType;
    }

    for (uint32_t memoryType = 0; memoryType < 32; ++memoryType)
    {
        if (memoryRequirements.memoryTypeBits & (1 << memoryType))
        {
            const VkMemoryType& type = deviceProperties.memoryTypes[memoryType];
            if ((type.propertyFlags & preferredFlags) == preferredFlags)
            {
                selectedType = memoryType;
                break;
            }
        }
    }

    if (selectedType == ~0u)
    {
        for (uint32_t memoryType = 0; memoryType < 32; ++memoryType)
        {
            if (memoryRequirements.memoryTypeBits & (1 << memoryType))
            {
                const VkMemoryType& type = deviceProperties.memoryTypes[memoryType];
                if ((type.propertyFlags & requiredFlags) == requiredFlags)
                {
                    selectedType = memoryType;
                    break;
                }
            }
        }
    }

    return selectedType;
}

void Device::BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize size)
{
    DebugTools::Verify(vkBindBufferMemory(device, buffer, memory, size));
}

void Device::BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize size)
{
    DebugTools::Verify(vkBindImageMemory(device, image, memory, size));
}

void Device::WaitIdle()
{
    vkDeviceWaitIdle(device);
}

void Device::CreateInternal()
{
    float priorityQueue[]{ 1.f };

    deviceQueueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        cachedPhysicalDevice->GetGraphicsFamilyIndex(),
        1,
        priorityQueue
    };

    deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        1,
        &deviceQueueCreateInfo,
        0,
        nullptr,
        (uint32_t)cachedDeviceExtensions.size(),
        cachedDeviceExtensions.data(),
        &cachedRequiredFeatures
    };

    if (DebugTools::Verify(vkCreateDevice(cachedPhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &device)) == VK_SUCCESS)
    {
        vkGetDeviceQueue(device, cachedPhysicalDevice->GetGraphicsFamilyIndex(), 0, &queue.GetVkQueueRef());
    }

}
