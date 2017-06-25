/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "BufferView.h"

VULKAN_NS_USING;

void BufferView::Create(VkDevice device, VkBufferViewCreateFlags flags, VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize range)
{
    bufferViewCreateInfo = {
        VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
        nullptr,
        flags,
        buffer,
        format,
        offset,
        range
    };

    DebugTools::Verify(vkCreateBufferView(device, &bufferViewCreateInfo, nullptr, &bufferView));
}

void BufferView::Destroy(VkDevice device)
{
    vkDestroyBufferView(device, bufferView, nullptr);
}
