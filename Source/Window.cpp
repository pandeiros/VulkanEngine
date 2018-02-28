/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Window.h"
#include "Instance.h"
#include "Core.h"

#include <array>
#include <string>
#include <algorithm>

VULKAN_NS_USING;

Window::Window(DevicePtr device, VkInstance instance, const WindowCreateInfo& windowCreateInfo)
    : VulkanClass(device), instance(instance), windowCreateInfo(windowCreateInfo)
{
    CreateOSWindow();
    CreateSurface();
    CreateSwapchain();
    CreateSwapchainImages();
    CreateDepthStencilImage();
    CreateRenderPass();
    CreateFramebuffer();
    CreateSynchronization();
}

Window::~Window()
{
    if (instance)
    {
        device->GetQueueRef().WaitIdle();

        DestroySynchronization();
        DestroyFramebuffer();
        DestroyRenderPass();
        DestroyDepthStencilImage();
        DestroySwapchainImages();
        DestroySwapchain();
        DestroySurface();
        DestroyOSWindow();
    }

    instance = VK_NULL_HANDLE;

    SetPendingKill(true);
}

//void Window::Create(Instance* instance, const WindowCreateInfo& windowCreateInfo)
//{
//    instance = instance;
//    this->windowCreateInfo = windowCreateInfo;
//
//    CreateOSWindow();
//    CreateSurface();
//    CreateSwapchain();
//    CreateSwapchainImages();
//    CreateDepthStencilImage();
//    CreateRenderPass();
//    CreateFramebuffer();
//    CreateSynchronization();
//}

//void Window::Destroy()
//{
//    if (instance)
//    {
//        device->GetQueueRef().WaitIdle();
//
//        DestroySynchronization();
//        DestroyFramebuffer();
//        DestroyRenderPass();
//        DestroyDepthStencilImage();
//        DestroySwapchainImages();
//        DestroySwapchain();
//        DestroySurface();
//        DestroyOSWindow();
//    }
//
//    instance = VK_NULL_HANDLE;
//    bPendingKill = false;
//}

bool Window::Update()
{
    UpdateOSWindow();

    return !IsPendingKill();
}

void Window::Close()
{
    SetPendingKill(true);
}

void Window::BeginRender()
{
    VK_PERFORMANCE_SECTION("Acquire next image");

    VK_VERIFY(vkAcquireNextImageKHR(device->GetVkDevice(), swapchain.GetVkSwapchain(), UINT64_MAX, semaphoreImageAcquired,
        VK_NULL_HANDLE, &activeSwapchainImageID));
//    VK_VERIFY(vkWaitForFences(device, 1, &fenceDraw, VK_TRUE, UINT64_MAX));
//    VK_VERIFY(vkResetFences(device, 1, &fenceDraw));

    device->GetQueueRef().WaitIdle();
    //VK_VERIFY(vkQueueWaitIdle(cacheddevice->GetQueueRef().GetVkQueueRef()));
}

void Window::EndRender(std::vector<VkSemaphore> waitSemaphores, std::vector<VkFence> waitFences)
{
    VkResult presentResult = VkResult::VK_RESULT_MAX_ENUM;

    VkPresentInfoKHR presentInfo = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        (uint32_t)waitSemaphores.size(),
        waitSemaphores.data(),
        1,
        &swapchain.GetVkSwapchain(),
        &activeSwapchainImageID,
        &presentResult
    };

    VkResult result;
    do
    {
        result = vkWaitForFences(device->GetVkDevice(), (uint32_t)waitFences.size(), waitFences.data(), VK_TRUE, UINT64_MAX);
    } while (result == VK_TIMEOUT);

    VK_VERIFY(vkQueuePresentKHR(device->GetQueueRef().GetVkQueue(), &presentInfo));
    VK_VERIFY(presentResult);
}

float Window::GetAspectRatio()
{
    return (float)GetSurfaceSize().width / (float)GetSurfaceSize().height;
}

VkExtent2D Window::GetSurfaceSize()
{
    return windowCreateInfo.surfaceSize;
}

VkRenderPass Window::GetRenderPass()
{
    return renderPass.GetVkRenderPass();
}

VkFramebuffer Window::GetActiveFramebuffer()
{
    return framebuffers[activeSwapchainImageID].GetVkFramebuffer();
}

void Window::CreateSurface()
{
    CreateOSSurface();

    VkPhysicalDevice physicalDevice = device->GetPhysicalDevice()->GetVkPhysicalDevice();
    uint32_t presentQueueFamilyIndex = device->GetPhysicalDevice()->GetPresentQueueFamilyIndex(surface);

    // Graphics family index may be changed to one supporting presenting and thus device should be reset.
    device->CheckPhysicalDeviceDirty();

    uint32_t formatCount = 0;
    VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr));
    VK_ASSERT(formatCount > 0, "Surface format missing.");

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    }
    else
    {
        VK_ASSERT(surfaceFormats.size() > 0, "Could not find any formats.");

        surfaceFormat = surfaceFormats[0];
    }
}

void Window::DestroySurface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

void Window::CreateSwapchain()
{
    VkPhysicalDevice physicalDevice = device->GetPhysicalDevice()->GetVkPhysicalDevice();

    VK_VERIFY(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

    //uint32_t presentModeCount = 0;
    //VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    //std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    //VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

    if (surfaceCapabilities.currentExtent.width < UINT32_MAX)
    {
        windowCreateInfo.surfaceSize = surfaceCapabilities.currentExtent;
    }
    else
    {
        windowCreateInfo.surfaceSize.width = Math::Clamp(windowCreateInfo.surfaceSize.width,
            surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);

        windowCreateInfo.surfaceSize.height = Math::Clamp(windowCreateInfo.surfaceSize.height,
            surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
    }

    // Image count
    uint32_t desiredSwapchainImageCount = Math::Clamp(2u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

    // Present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    // #SUPPORT Current Android driver supports only FIFO mode.
#ifndef __ANDROID__
    uint32_t presentModeCount = 0;
    VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

    for (VkPresentModeKHR& mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = mode;
        }
    }
#endif

    // Pre-transform.
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfaceCapabilities.currentTransform;
    }

    // Composite alpha mode
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] =
    {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
    };

    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); ++i)
    {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i])
        {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    // Clipped
    VkBool32 isClipped = VK_TRUE;

    // #SUPPORT
#ifdef __ANDROID__
    isClipped = VK_FALSE;
#endif

    // #TODO Handle scenario with graphics queue different than present queue.

    uint32_t imageArrayLayers = 1;

    swapchain.Create(
        device->GetVkDevice(),
        0,
        surface,
        desiredSwapchainImageCount,
        {
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            windowCreateInfo.surfaceSize,
            imageArrayLayers,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_SHARING_MODE_EXCLUSIVE
        },
        {},
        preTransform,
        compositeAlpha,
        presentMode,
        isClipped,
        VK_NULL_HANDLE);

    swapchainImageCount = swapchain.GetImageCount();
}

void Window::DestroySwapchain()
{
    if (instance)
    {
        swapchain.Destroy(device->GetVkDevice());
    }
}

void Window::CreateSwapchainImages()
{
    swapchainImages.resize(swapchainImageCount);
    swapchainImageViews.resize(swapchainImageCount);

    VK_VERIFY(vkGetSwapchainImagesKHR(device->GetVkDevice(), swapchain.GetVkSwapchain(), &swapchainImageCount, swapchainImages.data()));

    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        VkImageSubresourceRange subresourceRange = {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0, 1, 0, 1
        };

        swapchainImageViews[i].Create(device->GetVkDevice(), 0, swapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format,
            Image::GetRGBAComponentMapping(), subresourceRange);
    }
}

void Window::DestroySwapchainImages()
{
    for (ImageView& view : swapchainImageViews)
    {
        vkDestroyImageView(device->GetVkDevice(), view.GetVkImageView(), nullptr);
    }
}

void Window::CreateDepthStencilImage()
{
#ifdef __ANDROID__
    std::vector<VkFormat> desiredFormats
    {
        VK_FORMAT_D24_UNORM_S8_UINT,
    };
#else
    std::vector<VkFormat> desiredFormats
    {
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT
    };
#endif

    PhysicalDevice* physicalDevice = device->GetPhysicalDevice();

    VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
    VkImageTiling depthStencilImageTiling = VK_IMAGE_TILING_OPTIMAL;
    for (VkFormat& format : desiredFormats)
    {
        VkFormatProperties formatProperties = physicalDevice->GetFormatProperties(format);
        if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depthStencilFormat = format;
            break;
        }
        else if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            && depthStencilFormat == VK_FORMAT_UNDEFINED)
        {
            depthStencilImageTiling = VK_IMAGE_TILING_LINEAR;
            depthStencilFormat = format;
        }
    }

    VK_ASSERT(depthStencilFormat != VK_FORMAT_UNDEFINED, "Depth stencil format not selected.");

    bStencilAvailable = IsOfEnum<VkFormat>(depthStencilFormat, {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT
    });

    depthStencilImage.Create(device->GetVkDevice(), 0, VK_IMAGE_TYPE_2D, depthStencilFormat,
        { windowCreateInfo.surfaceSize.width, windowCreateInfo.surfaceSize.height, 1 },
        1, 1, VULKAN_SAMPLE_COUNT, depthStencilImageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_SHARING_MODE_EXCLUSIVE, {}, VK_IMAGE_LAYOUT_UNDEFINED);

    VkMemoryRequirements imageMemoryRequirements = depthStencilImage.GetMemoryRequirements(device->GetVkDevice());

    //uint32_t memoryIndex = physicalDevice->GetMemoryTypeIndex(&imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    uint32_t memoryIndex = physicalDevice->GetMemoryTypeIndex(&imageMemoryRequirements, 0);
    depthStencilImageMemory.Allocate(device->GetVkDevice(), imageMemoryRequirements.size, memoryIndex);
    depthStencilImageMemory.BindImageMemory(device->GetVkDevice(), depthStencilImage.GetVkImage(), 0);

    depthStencilImageView.Create(device->GetVkDevice(), 0, depthStencilImage.GetVkImage(), VK_IMAGE_VIEW_TYPE_2D, depthStencilFormat,
        Image::GetRGBAComponentMapping(),
        { VK_IMAGE_ASPECT_DEPTH_BIT | (bStencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0u), 0, 1, 0, 1 });
}

void Window::DestroyDepthStencilImage()
{
    if (instance)
    {
        depthStencilImageView.Destroy(device->GetVkDevice());
        depthStencilImageMemory.Free(device->GetVkDevice());
        depthStencilImage.Destroy(device->GetVkDevice());
    }
}

void Window::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> attachments;
    attachments.push_back(
        {
            0,
            surfaceFormat.format,
            VULKAN_SAMPLE_COUNT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        });
    attachments.push_back(
        {
            0,
            depthStencilImage.GetVkFormat(),
            VULKAN_SAMPLE_COUNT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_LOAD,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        });

    std::array<VkAttachmentReference, 1> subpassColorAttachments;
    subpassColorAttachments[0].attachment = 0;
    subpassColorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference subpassDepthStencilAttachment{};
    subpassDepthStencilAttachment.attachment = 1;
    subpassDepthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<VkSubpassDescription> subpasses;
    subpasses.push_back(
    {
        0,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        0,
        nullptr,
        (uint32_t)subpassColorAttachments.size(),
        subpassColorAttachments.data(),
        nullptr,
        &subpassDepthStencilAttachment,
        0,
        nullptr
    });

    renderPass.Create(device->GetVkDevice(), 0, attachments, subpasses, {});
}

void Window::DestroyRenderPass()
{
    if (instance)
    {
        renderPass.Destroy(device->GetVkDevice());
    }
}

void Window::CreateFramebuffer()
{
    framebuffers.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        std::vector<VkImageView> attachments(2);
        attachments[0] = swapchainImageViews[i].GetVkImageView();
        attachments[1] = depthStencilImageView.GetVkImageView();

        framebuffers[i].Create(device->GetVkDevice(), 0, renderPass.GetVkRenderPass(), attachments,
        { windowCreateInfo.surfaceSize.width, windowCreateInfo.surfaceSize.height, 1 });
    }
}

void Window::DestroyFramebuffer()
{
    if (instance)
    {
        for (Framebuffer& framebuffer : framebuffers)
        {
            framebuffer.Destroy(device->GetVkDevice());
        }
    }
}

void Window::CreateSynchronization()
{
    VkFenceCreateInfo fenceCreateInfo = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        nullptr,
        0
    };

    VK_VERIFY(vkCreateFence(device->GetVkDevice(), &fenceCreateInfo, nullptr, &fenceDraw));

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0
    };

    VK_VERIFY(vkCreateSemaphore(device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &semaphoreImageAcquired));
}

void Window::DestroySynchronization()
{
    vkDestroyFence(device->GetVkDevice(), fenceDraw, nullptr);
    vkDestroySemaphore(device->GetVkDevice(), semaphoreImageAcquired, nullptr);
}
