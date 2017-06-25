/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"

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
    /**
     * Default constructor.
     */
    ImageView() = default;

    /**
     * Default destructor.
     */
    ~ImageView() = default;

    void Create(VkDevice device, VkImageViewCreateFlags flags, VkImage image, VkImageViewType viewType,
        VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresourceRange);

    void Destroy(VkDevice device);

private:
    VkImageView imageView = VK_NULL_HANDLE;

    VkImageViewCreateInfo imageViewCreateInfo = {};
};

VULKAN_NS_END