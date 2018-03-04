/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file Swapchain.h
 */

VULKAN_NS_BEGIN

struct SwapchainImageInfo
{
    VkFormat imageFormat;
    VkColorSpaceKHR imageColorSpace;
    VkExtent2D imageExtent;
    uint32_t imageArrayLayers;
    VkImageUsageFlags imageUsage;
    VkSharingMode imageSharingMode;
};

/**
 * @class Swapchain
 */
class Swapchain
{
public:
    Swapchain() = default;

    ~Swapchain() = default;

    void Create(VkDevice device, VkSwapchainCreateFlagsKHR flags, VkSurfaceKHR surface, uint32_t desiredImageCount, SwapchainImageInfo imageinfo,
        std::vector<uint32_t> queueFamilyIndices, VkSurfaceTransformFlagBitsKHR preTransform, VkCompositeAlphaFlagBitsKHR compositeAlpha,
        VkPresentModeKHR presentMode, VkBool32 clipped, VkSwapchainKHR oldSwapchain);

    void Destroy(VkDevice device);

    VkSwapchainKHR& GetVkSwapchain();

    uint32_t GetImageCount() const;

protected:
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

    uint32_t imageCount;
};

VULKAN_NS_END