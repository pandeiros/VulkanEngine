/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Device.h"
#include "Core.h"

VULKAN_NS_USING;

Device::Device(PhysicalDevice* physicalDevice, std::vector<const char*> deviceExtensions, VkPhysicalDeviceFeatures requiredFeatures)
    : physicalDevice(physicalDevice), deviceExtensions(deviceExtensions), requiredFeatures(requiredFeatures)
{
    Init();
}

Device::~Device()
{
    Destroy();
}

void Device::Reset()
{
    Destroy();
    Init();

    physicalDevice->SetDirty(false);
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
    if (physicalDevice && physicalDevice->IsDirty())
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
    return physicalDevice;
}

void Device::FlushMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges)
{
    VK_VERIFY(vkFlushMappedMemoryRanges(device, (uint32_t)memoryRanges.size(), memoryRanges.data()));
}

void Device::InvalidateMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges)
{
    VK_VERIFY(vkInvalidateMappedMemoryRanges(device, (uint32_t)memoryRanges.size(), memoryRanges.data()));
}

uint32_t Device::GetMemoryTypeForImage(const VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags preferredFlags, VkMemoryPropertyFlags requiredFlags)
{
    uint32_t selectedType = ~0u;
    VkPhysicalDeviceMemoryProperties deviceProperties = {};

    if (physicalDevice)
    {
        deviceProperties = physicalDevice->GetPhysicalDeviceMemoryProperties();
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
    // #TODO FIX 0 to offset
    VK_VERIFY(vkBindBufferMemory(device, buffer, memory, 0));
}

void Device::BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize size)
{
    // #TODO FIX size to offset
    VK_VERIFY(vkBindImageMemory(device, image, memory, size));
}

void Device::WaitIdle()
{
    vkDeviceWaitIdle(device);
}

void Device::CreateSemaphore(VkSemaphore* semaphorePtr)
{
    if (device)
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo =
        {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            nullptr,
            0
        };

        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, semaphorePtr);
    }
}

void Device::DestroySemaphore(VkSemaphore semaphore)
{
    if (device && semaphore)
    {
        vkDestroySemaphore(device, semaphore, nullptr);
    }
}

VkPhysicalDeviceFeatures Device::GetRequiredFeatures() const
{
    return requiredFeatures;
}

void Device::Init()
{
    float priorityQueue[]{ 0.f };

    deviceQueueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        physicalDevice->GetGraphicsQueueFamilyIndex(),
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
        (uint32_t)deviceExtensions.size(),
        deviceExtensions.data(),
        &requiredFeatures
    };

    if (VK_VERIFY(vkCreateDevice(physicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &device)) == VK_SUCCESS)
    {
        vkGetDeviceQueue(device, physicalDevice->GetGraphicsQueueFamilyIndex(), 0, queue.GetVkQueuePtr());
    }

}
