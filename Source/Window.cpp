/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Window.h"
#include "Instance.h"

#include <array>

VULKAN_NS_USING;

void Window::Create(Instance* instance, const WindowCreateInfo& windowCreateInfo)
{
    cachedInstance = instance;
    this->windowCreateInfo = windowCreateInfo;

    CreateOSWindow();
    CreateSurface();
    CreateSwapchain();
    CreateSwapchainImages();
    CreateDepthStencilImage();
    CreateRenderPass();
    CreateFramebuffer();
    CreateSynchronization();
}

void Window::Destroy()
{
    if (cachedInstance)
    {
        cachedInstance->GetDeviceRef().GetQueueRef().WaitIdle();

        DestroySynchronization();
        DestroyFramebuffer();
        DestroyRenderPass();
        DestroyDepthStencilImage();
        DestroySwapchainImages();
        DestroySwapchain();
        DestroySurface();
        DestroyOSWindow();
    }

    cachedInstance = VK_NULL_HANDLE;
    bIsValid = false;
}

bool Window::Update()
{
    UpdateOSWindow();

    return bIsValid;
}

void Window::Close()
{
    bIsValid = false;
}

void Window::BeginRender()
{
    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

    DebugTools::Verify(vkAcquireNextImageKHR(device, swapchain.GetVkSwapchain(), UINT64_MAX, VK_NULL_HANDLE, fenceSwapchainImageAvailable, &activeSwapchainImageID));
    DebugTools::Verify(vkWaitForFences(device, 1, &fenceSwapchainImageAvailable, VK_TRUE, UINT64_MAX));
    DebugTools::Verify(vkResetFences(device, 1, &fenceSwapchainImageAvailable));
    DebugTools::Verify(vkQueueWaitIdle(cachedInstance->GetDeviceRef().GetQueueRef().GetVkQueueRef()));
}

void Window::EndRender(std::vector<VkSemaphore> waitSemaphores)
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

    DebugTools::Verify(vkQueuePresentKHR(cachedInstance->GetDeviceRef().GetQueueRef().GetVkQueueRef(), &presentInfo));
    DebugTools::Verify(presentResult);
}

VkExtent2D Window::GetSurfaceSize()
{
    return windowCreateInfo.surfaceSize;
}

VkRenderPass Window::GetRenderPass()
{
    return renderPass.GetVkRenderPass();
}

Framebuffer& Window::GetActiveFramebuffer()
{
    return framebuffers[activeSwapchainImageID];
}

void Window::CreateSurface()
{
    CreateOSSurface();

    VkPhysicalDevice physicalDevice = cachedInstance->GetDeviceRef().GetPhysicalDevice()->GetVkPhysicalDevice();
    uint32_t presentQueueFamilyIndex = cachedInstance->GetDeviceRef().GetPhysicalDevice()->GetPresentQueueFamilyIndex(surface);

    // Graphics family index may be changed to one supporting presenting and thus device should be reset.
    cachedInstance->GetDeviceRef().CheckPhysicalDeviceDirty();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    if (surfaceCapabilities.currentExtent.width < UINT32_MAX)
    {
        windowCreateInfo.surfaceSize = surfaceCapabilities.currentExtent;
    }

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    DebugTools::Assert(formatCount, "Surface format missing.");

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
    if (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    }
    else
    {
        DebugTools::Assert(surfaceFormats.size() > 0, "Could not find any formats.");

        surfaceFormat = surfaceFormats[0];
    }
}

void Window::DestroySurface()
{
    vkDestroySurfaceKHR(cachedInstance->GetVkInstance(), surface, nullptr);
}

void Window::CreateSwapchain()
{
    if (swapchainImageCount < surfaceCapabilities.minImageCount + 1)
    {
        swapchainImageCount = surfaceCapabilities.minImageCount + 1;
    }

    if (surfaceCapabilities.maxImageCount > 0)
    {
        if (swapchainImageCount > surfaceCapabilities.maxImageCount)
        {
            swapchainImageCount = surfaceCapabilities.maxImageCount;
        }
    }


    VkPhysicalDevice physicalDevice = cachedInstance->GetDeviceRef().GetPhysicalDevice()->GetVkPhysicalDevice();
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t presentModeCount = 0;
    DebugTools::Verify(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    DebugTools::Verify(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

    // #TODO Maybe pass required modes as arguments to Create function?
    for (VkPresentModeKHR& mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            presentMode = mode;
    }

    uint32_t imageArrayLayers = 1;

    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

    swapchain.Create(device,
        0,
        surface,
        swapchainImageCount,
        {
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            windowCreateInfo.surfaceSize,
            imageArrayLayers,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_SHARING_MODE_EXCLUSIVE
        },
        {},
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        presentMode,
        VK_TRUE,
        VK_NULL_HANDLE);
}

void Window::DestroySwapchain()
{
    if (cachedInstance)
    {
        VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

        swapchain.Destroy(device);
    }
}

void Window::CreateSwapchainImages()
{
    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

    swapchainImages.resize(swapchainImageCount);
    swapchainImageViews.resize(swapchainImageCount);

    DebugTools::Verify(vkGetSwapchainImagesKHR(device, swapchain.GetVkSwapchain(), &swapchainImageCount, swapchainImages.data()));

    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        VkImageSubresourceRange subresourceRange = {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0, 1, 0, 1
        };

        //ImageView imageView;

        swapchainImageViews[i].Create(device, 0, swapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format,
            Image::GetIdentityComponentMapping(), subresourceRange);
    }
}

void Window::DestroySwapchainImages()
{
    for (ImageView& view : swapchainImageViews)
    {
        vkDestroyImageView(cachedInstance->GetDeviceRef().GetVkDevice(), view.GetVkImageView(), nullptr);
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
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D16_UNORM
    };
#endif

    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();
    PhysicalDevice* physicalDevice = cachedInstance->GetDeviceRef().GetPhysicalDevice();

    VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
    for (VkFormat& format : desiredFormats)
    {
        VkFormatProperties formatProperties{};
        vkGetPhysicalDeviceFormatProperties(physicalDevice->GetVkPhysicalDevice(), format, &formatProperties);
        if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depthStencilFormat = format;
            break;
        }
    }

    DebugTools::Assert(depthStencilFormat != VK_FORMAT_UNDEFINED, "Depth stencil format not selected.");

    bStencilAvailable = IsOfEnum<VkFormat>(depthStencilFormat, {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_S8_UINT });

    depthStencilImage.Create(device, 0, VK_IMAGE_TYPE_2D, depthStencilFormat,
        { windowCreateInfo.surfaceSize.width, windowCreateInfo.surfaceSize.height, 1 },
        1, 1, VULKAN_SAMPLE_COUNT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_SHARING_MODE_EXCLUSIVE, {}, VK_IMAGE_LAYOUT_UNDEFINED);

    VkMemoryRequirements imageMemoryRequirements = depthStencilImage.GetMemoryRequirements(device);

    uint32_t memoryIndex = physicalDevice->GetMemoryTypeIndex(&imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    depthStencilImageMemory.Allocate(device, imageMemoryRequirements.size, memoryIndex);
    depthStencilImageMemory.BindImageMemory(device, depthStencilImage.GetVkImage(), 0);

    depthStencilImageView.Create(device, 0, depthStencilImage.GetVkImage(), VK_IMAGE_VIEW_TYPE_2D, depthStencilFormat, Image::GetIdentityComponentMapping(),
        { VK_IMAGE_ASPECT_DEPTH_BIT | (bStencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0u), 0, 1, 0, 1 });
}

void Window::DestroyDepthStencilImage()
{
    if (cachedInstance)
    {
        VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

        depthStencilImageView.Destroy(device);
        depthStencilImageMemory.Free(device);
        depthStencilImage.Destroy(device);
    }
}

void Window::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> attachments;
    attachments.push_back(
    {
        0,
        depthStencilImage.GetVkFormat(),
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    });

    attachments.push_back(
    {
        0,
        surfaceFormat.format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    });

    VkAttachmentReference subpassDepthStencilAttachment{};
    subpassDepthStencilAttachment.attachment = 0;
    subpassDepthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 1> subpassColorAttachments;
    subpassColorAttachments[0].attachment = 1;
    subpassColorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

    DebugTools::Assert(cachedInstance);
    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();
    renderPass.Create(device, 0, attachments, subpasses, {});
}

void Window::DestroyRenderPass()
{
    if (cachedInstance)
    {
        VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

        renderPass.Destroy(device);
    }
}

void Window::CreateFramebuffer()
{
    framebuffers.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        std::vector<VkImageView> attachments(2);
        attachments[0] = depthStencilImageView.GetVkImageView();
        attachments[1] = swapchainImageViews[i].GetVkImageView();

        DebugTools::Assert(cachedInstance);
        VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

        framebuffers[i].Create(device, 0, renderPass.GetVkRenderPass(), attachments,
        { windowCreateInfo.surfaceSize.width, windowCreateInfo.surfaceSize.height, 1 });
    }
}

void Window::DestroyFramebuffer()
{
    if (cachedInstance)
    {
        VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

        for (Framebuffer& framebuffer : framebuffers)
        {
            framebuffer.Destroy(device);
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

    vkCreateFence(cachedInstance->GetDeviceRef().GetVkDevice(), &fenceCreateInfo, nullptr, &fenceSwapchainImageAvailable);
}

void Window::DestroySynchronization()
{
    vkDestroyFence(cachedInstance->GetDeviceRef().GetVkDevice(), fenceSwapchainImageAvailable, nullptr);
}
