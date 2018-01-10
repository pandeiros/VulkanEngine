/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Image.h"
#include "Core.h"

VULKAN_NS_USING;

void Image::Create(VkDevice device, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkExtent3D extent,
    uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
    VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndices, VkImageLayout initialLayout)
{
    CheckExtent(type, extent);

    imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        flags,
        type,
        format,
        extent,
        mipLevels,
        arrayLayers,
        samples,
        tiling,
        usage,
        sharingMode,
        (uint32_t)queueFamilyIndices.size(),
        queueFamilyIndices.data(),
        initialLayout
    };

    VK_VERIFY(vkCreateImage(device, &imageCreateInfo, nullptr, &image));
}

void Image::Destroy(VkDevice device)
{
    if (device)
    {
        vkDestroyImage(device, image, nullptr);
    }

    image = VK_NULL_HANDLE;
}

VkImage Image::GetVkImage()
{
    return image;
}

VkFormat Image::GetVkFormat()
{
    return imageCreateInfo.format;
}

VkSubresourceLayout Image::GetImageSubresourceLayout(VkDevice device, const VkImageSubresource subresource)
{
    VkSubresourceLayout layout;

    if (image)
    {
        vkGetImageSubresourceLayout(device, image, &subresource, &layout);
    }

    return layout;
}

VkMemoryRequirements Image::GetMemoryRequirements(VkDevice device) const
{
    VkMemoryRequirements requirements;

    if (image)
    {
        vkGetImageMemoryRequirements(device, image, &requirements);
    }

    return requirements;
}

VkComponentMapping Image::GetIdentityComponentMapping()
{
    return { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
}

void Image::CheckExtent(VkImageType type, VkExtent3D& outExtent)
{
    if (type == VK_IMAGE_TYPE_1D)
    {
        outExtent.height = outExtent.depth = 1u;
    }
    else if (type == VK_IMAGE_TYPE_2D)
    {
        outExtent.depth = 1u;
    }
}

void SparseImage::Create(VkDevice device, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkExtent3D extent,
    uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
    VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndices, VkImageLayout initialLayout)
{
    //Image::Create(device, flags, type, format, extent, mipLevels, arrayLayers, samples, tiling, usage, sharingMode, queueFamilyIndices, initialLayout);

    CheckExtent(type, extent);

    imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        flags,
        type,
        format,
        extent,
        mipLevels,
        arrayLayers,
        samples,
        tiling,
        usage,
        sharingMode,
        (uint32_t)queueFamilyIndices.size(),
        queueFamilyIndices.data(),
        initialLayout
    };

    if (VK_VERIFY(vkCreateImage(device, &imageCreateInfo, nullptr, &image)) == VK_SUCCESS
        && flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT)
    {
        uint32_t requirementsCount = 0;
        vkGetImageSparseMemoryRequirements(device, image, &requirementsCount, nullptr);
        sparseMemoryRequirements.resize(requirementsCount);
        vkGetImageSparseMemoryRequirements(device, image, &requirementsCount, sparseMemoryRequirements.data());
    }
}

std::vector<VkSparseImageMemoryRequirements>& SparseImage::GetSparseMemoryRequirements(VkDevice device)
{
    return sparseMemoryRequirements;
}
