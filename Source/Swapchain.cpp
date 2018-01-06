/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Swapchain.h"

VULKAN_NS_USING;

void Swapchain::Create(VkDevice device, VkSwapchainCreateFlagsKHR flags, VkSurfaceKHR surface, uint32_t desiredImageCount, SwapchainImageInfo imageinfo,
    std::vector<uint32_t> queueFamilyIndices, VkSurfaceTransformFlagBitsKHR preTransform, VkCompositeAlphaFlagBitsKHR compositeAlpha,
    VkPresentModeKHR presentMode, VkBool32 clipped, VkSwapchainKHR oldSwapchain)
{
    swapchainCreateInfo = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        surface,
        desiredImageCount,
        imageinfo.imageFormat,
        imageinfo.imageColorSpace,
        imageinfo.imageExtent,
        imageinfo.imageArrayLayers,
        imageinfo.imageUsage,
        imageinfo.imageSharingMode,
        (uint32_t)queueFamilyIndices.size(),
        queueFamilyIndices.data(),
        preTransform,
        compositeAlpha,
        presentMode,
        clipped,
        oldSwapchain
    };

    DebugTools::Verify(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));
    DebugTools::Verify(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
}

void Swapchain::Destroy(VkDevice device)
{
    if (device)
    {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }

    swapchain = VK_NULL_HANDLE;
}

VkSwapchainKHR& Swapchain::GetVkSwapchain()
{
    return swapchain;
}

uint32_t Swapchain::GetImageCount() const
{
    return imageCount;
}
