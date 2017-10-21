/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
#include "PhysicalDevice.h"
#include "Queue.h"

#include <vector>

/**
 * @file Device.h
 */

VULKAN_NS_BEGIN

/**
 * @class Device
 */
class Device
{
public:
    /**
     * Default constructor.
     */
    Device() = default;

    /**
     * Default destructor.
     */
    ~Device() = default;

    void Create(PhysicalDevice& physicalDevice, std::vector<const char*> deviceExtensions = {},
        VkPhysicalDeviceFeatures requiredFeatures = {});

    void Destroy();

    VkDevice GetVkDevice();
    Queue& GetQueueRef();
    PhysicalDevice* GetPhysicalDevice();

    void FlushMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges);
    void InvalidateMappedMemoryRanges(std::vector<VkMappedMemoryRange> memoryRanges);

    uint32_t GetMemoryTypeForImage(const VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags preferredFlags, VkMemoryPropertyFlags requiredFlags);

    void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize size);
    void BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize size);

    void WaitIdle();

private:
    VkDevice device;

    Queue queue;

    PhysicalDevice* cachedPhysicalDevice;

    VkDeviceCreateInfo deviceCreateInfo = {};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
};

VULKAN_NS_END