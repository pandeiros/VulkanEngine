/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Image.h"

VULKAN_NS_USING;

void Image::Create(VkDevice device, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkExtent3D extent,
    uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
    VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndices, VkImageLayout initialLayout)
{
    checkExtent(type, extent);

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

    DebugTools::Verify(vkCreateImage(device, &imageCreateInfo, nullptr, &image));
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

void Image::Destroy(VkDevice device)
{
    vkDestroyImage(device, image, nullptr);
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

void Image::checkExtent(VkImageType type, VkExtent3D& outExtent)
{
    if (type == VK_IMAGE_TYPE_1D)
    {
        outExtent.height = outExtent.depth = 1u;
    }
    else if (type = VK_IMAGE_TYPE_2D)
    {
        outExtent.depth = 1u;
    }
}

void SparseImage::Create(VkDevice device, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkExtent3D extent,
    uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
    VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndices, VkImageLayout initialLayout)
{
    //Image::Create(device, flags, type, format, extent, mipLevels, arrayLayers, samples, tiling, usage, sharingMode, queueFamilyIndices, initialLayout);

    checkExtent(type, extent);

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

    if (DebugTools::Verify(vkCreateImage(device, &imageCreateInfo, nullptr, &image)) == VK_SUCCESS
        && flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT)
    {
        uint32_t requirementsCount = 0;
        vkGetImageSparseMemoryRequirements(device, image, &requirementsCount, nullptr);
        sparseMemoryRequirements.resize(requirementsCount);
        vkGetImageSparseMemoryRequirements(device, image, &requirementsCount, sparseMemoryRequirements.data());

        //uint32_t sparsePropertiesCount = 0;
        //vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, &sparsePropertiesCount, nullptr);
        //queueFamilyProperties.resize(sparsePropertiesCount);
        //vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, &sparsePropertiesCount, sparseImageFormatProperties.data());
    }
}

std::vector<VkSparseImageMemoryRequirements>& SparseImage::GetSparseMemoryRequirements(VkDevice device)
{
    return sparseMemoryRequirements;
}
