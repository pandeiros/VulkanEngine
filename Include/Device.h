/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
#include "PhysicalDevice.h"
#include "Queue.h"

//#include <vector>

/**
 * @file Device.h
 */

VULKAN_NS_BEGIN

/**
 * @class Device
 */
class Device : public VulkanClass
{
public:
    Device(PhysicalDevice* physicalDevice, std::vector<const char*> deviceExtensions = {},
        VkPhysicalDeviceFeatures requiredFeatures = {});

    ~Device();

    void Reset();
    virtual void Destroy() override;
    void CheckPhysicalDeviceDirty();

    VkDevice GetVkDevice();
    Queue& GetQueueRef();
    PhysicalDevice* GetPhysicalDevice();

    void FlushMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges);
    void InvalidateMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges);

    uint32_t GetMemoryTypeForImage(const VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags preferredFlags, VkMemoryPropertyFlags requiredFlags);

    void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize size);
    void BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize size);

    void WaitIdle();

    void CreateSemaphore(VkSemaphore* semaphorePointer);
    void DestroySemaphore(VkSemaphore semaphore);

private:
    void Init() override;

    VkDevice device = VK_NULL_HANDLE;

    Queue queue;

    PhysicalDevice* physicalDevice;

    VkDeviceCreateInfo deviceCreateInfo = {};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};

    // Properties used to initialize VkDevice.
    std::vector<const char*> deviceExtensions;
    VkPhysicalDeviceFeatures requiredFeatures;
};

typedef std::shared_ptr<Device> DevicePtr;

VULKAN_NS_END