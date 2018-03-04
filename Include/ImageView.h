/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file ImageView.h
 */

VULKAN_NS_BEGIN

/**
 * @class ImageView
 */
class ImageView
{
public:
    ImageView() = default;

    ~ImageView() = default;

    void Create(VkDevice device, VkImageViewCreateFlags flags, VkImage image, VkImageViewType viewType,
        VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresourceRange);

    void Destroy(VkDevice device);

    VkImageView GetVkImageView();

private:
    VkImageView imageView = VK_NULL_HANDLE;

    VkImageViewCreateInfo imageViewCreateInfo = {};
};

VULKAN_NS_END