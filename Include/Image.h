/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file Image.h
 */

VULKAN_NS_BEGIN

/**
 * @class Image
 */
class Image
{
public:
    Image() = default;

    ~Image() = default;

    virtual void Create(VkDevice device, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkExtent3D extent,
        uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
        VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndices, VkImageLayout initialLayout);

    void Destroy(VkDevice device);

    VkImage GetVkImage();
    VkFormat GetVkFormat();

    VkSubresourceLayout GetImageSubresourceLayout(VkDevice device, const VkImageSubresource subresource);
    VkMemoryRequirements GetMemoryRequirements(VkDevice device) const;

    static VkComponentMapping GetIdentityComponentMapping();
    static VkComponentMapping GetRGBAComponentMapping();

protected:
    VkImage image = VK_NULL_HANDLE;

    VkImageCreateInfo imageCreateInfo = {};

    /** Forces proper extent values for 1D and 2D images. */
    void CheckExtent(VkImageType type, VkExtent3D& outExtent);
};

/**
 * @class SparseImage
 */
class SparseImage : public Image
{
public:
    SparseImage() = default;

    ~SparseImage() = default;

    virtual void Create(VkDevice device, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkExtent3D extent,
        uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
        VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndices, VkImageLayout initialLayout);

    std::vector<VkSparseImageMemoryRequirements>& GetSparseMemoryRequirements(VkDevice device);

private:
    std::vector<VkSparseImageMemoryRequirements> sparseMemoryRequirements;
};

VULKAN_NS_END