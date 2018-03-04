/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file BufferView.h
 */

VULKAN_NS_BEGIN

/**
 * @class BufferView
 */
class BufferView
{
public:
    BufferView() = default;

    ~BufferView() = default;

    void Create(VkDevice device, VkBufferViewCreateFlags flags, VkBuffer buffer, VkFormat format,
        VkDeviceSize offset, VkDeviceSize range);

    void Destroy(VkDevice device);

private:
    VkBufferView bufferView = VK_NULL_HANDLE;

    VkBufferViewCreateInfo bufferViewCreateInfo = {};
};

VULKAN_NS_END