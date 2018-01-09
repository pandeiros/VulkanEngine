/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "ImageView.h"
#include "Core.h"

VULKAN_NS_USING;

void ImageView::Create(VkDevice device, VkImageViewCreateFlags flags, VkImage image, VkImageViewType viewType,
    VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresourceRange)
{
    imageViewCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        nullptr,
        flags,
        image,
        viewType,
        format,
        components,
        subresourceRange
    };

    DebugTools::Verify(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView));
}

void ImageView::Destroy(VkDevice device)
{
    if (device)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }
}

VkImageView ImageView::GetVkImageView()
{
    return imageView;
}
