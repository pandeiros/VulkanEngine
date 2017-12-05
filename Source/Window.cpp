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

    DebugTools::Verify(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, VK_NULL_HANDLE, fenceSwapchainImageAvailable, &activeSwapchainImageID));
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
        &swapchain,
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
    return renderPass;
}

VkFramebuffer Window::GetActiveFramebuffer()
{
    return framebuffers[activeSwapchainImageID];
}

void Window::CreateSurface()
{
    CreateOSSurface();

    VkPhysicalDevice physicalDevice = cachedInstance->GetDeviceRef().GetPhysicalDevice()->GetVkPhysicalDevice();
    uint32_t queueFamilyIndex = cachedInstance->GetDeviceRef().GetPhysicalDevice()->GetGraphicsFamilyIndex();

    VkBool32 bWSISupported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &bWSISupported);

    /*if (!bWSISupported)
    {
    assert(0 && "WSI not supported.");
    std::exit(-1);
    }*/

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
        swapchainImageCount = surfaceCapabilities.minImageCount + 1;

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

    swapchainCreateInfo = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        surface,
        swapchainImageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        windowCreateInfo.surfaceSize,
        imageArrayLayers,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        nullptr,
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        presentMode,
        VK_TRUE,
        VK_NULL_HANDLE
    };

    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

    DebugTools::Verify(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));
    DebugTools::Verify(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr));
}

void Window::DestroySwapchain()
{
    vkDestroySwapchainKHR(cachedInstance->GetDeviceRef().GetVkDevice(), swapchain, nullptr);
}

void Window::CreateSwapchainImages()
{
    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

    swapchainImages.resize(swapchainImageCount);
    swapchainImageViews.resize(swapchainImageCount);

    DebugTools::Verify(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data()));

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
        1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_SHARING_MODE_EXCLUSIVE, {}, VK_IMAGE_LAYOUT_UNDEFINED);

    VkMemoryRequirements imageMemoryRequirements{};
    vkGetImageMemoryRequirements(device, depthStencilImage.GetVkImage(), &imageMemoryRequirements);

    uint32_t memoryIndex = physicalDevice->GetMemoryTypeIndex(&imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryIndex;
    vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &depthStencilImageMemory);
    vkBindImageMemory(device, depthStencilImage.GetVkImage(), depthStencilImageMemory, 0);

    depthStencilImageView.Create(device, 0, depthStencilImage.GetVkImage(), VK_IMAGE_VIEW_TYPE_2D, depthStencilFormat, Image::GetIdentityComponentMapping(),
        { VK_IMAGE_ASPECT_DEPTH_BIT | (bStencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0u), 0, 1, 0, 1 });
}

void Window::DestroyDepthStencilImage()
{
    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();

    vkDestroyImageView(device, depthStencilImageView.GetVkImageView(), nullptr);
    vkFreeMemory(device, depthStencilImageMemory, nullptr);
    vkDestroyImage(device, depthStencilImage.GetVkImage(), nullptr);
}

void Window::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 2> attachments{};

    attachments[0].flags = 0;
    attachments[0].format = depthStencilImage.GetVkFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachments[1].flags = 0;
    attachments[1].format = surfaceFormat.format;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference subpassDepthStencilAttachment{};
    subpassDepthStencilAttachment.attachment = 0;
    subpassDepthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 1> subpassColorAttachments;
    subpassColorAttachments[0].attachment = 1;
    subpassColorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::array<VkSubpassDescription, 1> subpasses{};
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].colorAttachmentCount = (uint32_t)subpassColorAttachments.size();
    subpasses[0].pColorAttachments = subpassColorAttachments.data();
    subpasses[0].pDepthStencilAttachment = &subpassDepthStencilAttachment;

    VkRenderPassCreateInfo renderPassCreateInfo = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)attachments.size(),
        attachments.data(),
        (uint32_t)subpasses.size(),
        subpasses.data(),
        0,
        nullptr
    };

    VkDevice device = cachedInstance->GetDeviceRef().GetVkDevice();
    DebugTools::Verify(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));
}

void Window::DestroyRenderPass()
{
    vkDestroyRenderPass(cachedInstance->GetDeviceRef().GetVkDevice(), renderPass, nullptr);
}

void Window::CreateFramebuffer()
{
    framebuffers.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        std::array<VkImageView, 2> attachments{};
        attachments[0] = depthStencilImageView.GetVkImageView();
        attachments[1] = swapchainImageViews[i].GetVkImageView();

        VkFramebufferCreateInfo framebufferCreateInfo = {
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            nullptr,
            0,
            renderPass,
            (uint32_t)attachments.size(),
            attachments.data(),
            windowCreateInfo.surfaceSize.width,
            windowCreateInfo.surfaceSize.height,
            1
        };

        vkCreateFramebuffer(cachedInstance->GetDeviceRef().GetVkDevice(), &framebufferCreateInfo, nullptr, &framebuffers[i]);
    }
}

void Window::DestroyFramebuffer()
{
    for (VkFramebuffer& framebuffer : framebuffers)
    {
        vkDestroyFramebuffer(cachedInstance->GetDeviceRef().GetVkDevice(), framebuffer, nullptr);
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
