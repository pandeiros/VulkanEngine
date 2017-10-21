#include "Rendering/Windows_old.h"
#include "Rendering/Renderer.h"
#include <cassert>
#include <array>

VULKAN_NS_USING;

WindowOld::WindowOld(Renderer * Renderer, uint32_t SizeX, uint32_t SizeY, std::string Name)
{
    MainRenderer = Renderer;
    SurfaceSizeX = SizeX;
    SurfaceSizeY = SizeY;
    WindowName = Name;

    //CreateOSWindow();
    CreateSurface();
    CreateSwapchain();
    CreateSwapchainImages();
    CreateDepthStencilImage();
    CreateRenderPass();
    CreateFramebuffer();
    CreateSynchronization();
}

WindowOld::~WindowOld()
{
    vkQueueWaitIdle(MainRenderer->GetQueue());

    DestroySynchronization();
    DestroyFramebuffer();
    DestroyRenderPass();
    DestroyDepthStencilImage();
    DestroySwapchainImages();
    DestroySwapchain();
    DestroySurface();
    //DestroyOSWindow();
}

void WindowOld::Close()
{
    IsValid = false;
}

bool WindowOld::Update()
{
    //UpdateOSWindow();
    return IsValid;
}

void WindowOld::BeginRender()
{
    (vkAcquireNextImageKHR(MainRenderer->GetDevice(), Swapchain, UINT64_MAX, VK_NULL_HANDLE, SwapchainImageAvailable, &ActiveSwapchainImageID));
    (vkWaitForFences(MainRenderer->GetDevice(), 1, &SwapchainImageAvailable, VK_TRUE, UINT64_MAX));
    (vkResetFences(MainRenderer->GetDevice(), 1, &SwapchainImageAvailable));
    (vkQueueWaitIdle(MainRenderer->GetQueue()));
}

void WindowOld::EndRender(std::vector<VkSemaphore> WaitSemaphores)
{
    VkResult PresentResult = VkResult::VK_RESULT_MAX_ENUM;

    VkPresentInfoKHR PresentInfo {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.waitSemaphoreCount = WaitSemaphores.size();
    PresentInfo.pWaitSemaphores = WaitSemaphores.data();
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &Swapchain;
    PresentInfo.pImageIndices = &ActiveSwapchainImageID;
    PresentInfo.pResults = &PresentResult;

    (vkQueuePresentKHR(MainRenderer->GetQueue(), &PresentInfo));
    (PresentResult);
}

VkRenderPass WindowOld::GetRenderPass()
{
    return RenderPass;
}

VkFramebuffer WindowOld::GetActiveFramebuffer()
{
    return Framebuffers[ActiveSwapchainImageID];
}

VkExtent2D WindowOld::GetSurfaceSize()
{
    return { SurfaceSizeX, SurfaceSizeY };
}

void WindowOld::CreateSurface()
{
    //CreateOSSurface();

    auto GPU = MainRenderer->GetPhysicalDevice();

    VkBool32 IsWSISupported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(GPU, MainRenderer->GetGraphicsFamilyIndex(), Surface, &IsWSISupported);

    /*if (!IsWSISupported)
    {
        assert(0 && "WSI not supported.");
        std::exit(-1);
    }*/

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, Surface, &SurfaceCapabilities);

    if (SurfaceCapabilities.currentExtent.width < UINT32_MAX)
    {
        SurfaceSizeX = SurfaceCapabilities.currentExtent.width;
        SurfaceSizeY = SurfaceCapabilities.currentExtent.height;
    }

    uint32_t FormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, Surface, &FormatCount, nullptr);
    if (!FormatCount)
    {
        assert(0 && "Surface format missing.");
        std::exit(-1);
    }

    std::vector<VkSurfaceFormatKHR> SurfaceFormats(FormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, Surface, &FormatCount, SurfaceFormats.data());
    if (SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        SurfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    }
    else
    {
        SurfaceFormat = SurfaceFormats[0];
    }
}

void WindowOld::DestroySurface()
{
    vkDestroySurfaceKHR(MainRenderer->GetInstance(), Surface, nullptr);
}

void WindowOld::CreateSwapchain()
{
    if (SwapchainImageCount < SurfaceCapabilities.minImageCount + 1)
        SwapchainImageCount = SurfaceCapabilities.minImageCount + 1;

    if (SurfaceCapabilities.maxImageCount > 0)
    {
        if (SwapchainImageCount > SurfaceCapabilities.maxImageCount)
        {
            SwapchainImageCount = SurfaceCapabilities.maxImageCount;
        }
    }


    VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t PresentModeCount = 0;
    (vkGetPhysicalDeviceSurfacePresentModesKHR(MainRenderer->GetPhysicalDevice(), Surface, &PresentModeCount, nullptr));
    std::vector<VkPresentModeKHR> PresentModes(PresentModeCount);
    (vkGetPhysicalDeviceSurfacePresentModesKHR(MainRenderer->GetPhysicalDevice(), Surface, &PresentModeCount, PresentModes.data()));

    for (auto & Mode : PresentModes)
    {
        if (Mode == VK_PRESENT_MODE_MAILBOX_KHR)
            PresentMode = Mode;
    }

    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = Surface;
    SwapchainCreateInfo.minImageCount = SwapchainImageCount;
    SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapchainCreateInfo.imageExtent.width = SurfaceSizeX;
    SwapchainCreateInfo.imageExtent.height = SurfaceSizeY;
    SwapchainCreateInfo.imageArrayLayers = 1;
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainCreateInfo.queueFamilyIndexCount = 0;
    SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
    SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.presentMode = PresentMode;
    SwapchainCreateInfo.clipped = VK_TRUE;
    SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    (vkCreateSwapchainKHR(MainRenderer->GetDevice(), &SwapchainCreateInfo, nullptr, &Swapchain));

    (vkGetSwapchainImagesKHR(MainRenderer->GetDevice(), Swapchain, &SwapchainImageCount, nullptr));
}

void WindowOld::DestroySwapchain()
{
    vkDestroySwapchainKHR(MainRenderer->GetDevice(), Swapchain, nullptr);
}

void WindowOld::CreateSwapchainImages()
{
    SwapchainImages.resize(SwapchainImageCount);
    SwapchainImageViews.resize(SwapchainImageCount);

    (vkGetSwapchainImagesKHR(MainRenderer->GetDevice(), Swapchain, &SwapchainImageCount, SwapchainImages.data()));

    for (uint32_t i = 0; i < SwapchainImageCount; ++i)
    {
        VkImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = SwapchainImages[i];
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = SurfaceFormat.format;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;

        (vkCreateImageView(MainRenderer->GetDevice(), &ImageViewCreateInfo, nullptr, &SwapchainImageViews[i]));
    }
}

void WindowOld::DestroySwapchainImages()
{
    for (auto & View : SwapchainImageViews)
    {
        vkDestroyImageView(MainRenderer->GetDevice(), View, nullptr);
    }
}

void WindowOld::CreateDepthStencilImage()
{
    std::vector<VkFormat> DesiredFormats
    {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D16_UNORM
    };

    for (auto & Format : DesiredFormats)
    {
        VkFormatProperties FormatProperties {};
        vkGetPhysicalDeviceFormatProperties(MainRenderer->GetPhysicalDevice(), Format, &FormatProperties);
        if (FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            DepthStencilFormat = Format;

            break;
        }
    }

    if (DepthStencilFormat == VK_FORMAT_UNDEFINED)
    {
        assert(0 && "Depth stencil format not selected.");
        std::exit(-1);
    }

    IsStencilAvailable = IsOfEnum<VkFormat>(DepthStencilFormat, {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_S8_UINT});

    VkImageCreateInfo ImageCreateInfo {};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.flags = 0;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = DepthStencilFormat;
    ImageCreateInfo.extent.width = SurfaceSizeX;
    ImageCreateInfo.extent.height = SurfaceSizeY;
    ImageCreateInfo.extent.depth = 1;
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.pQueueFamilyIndices = nullptr;
    ImageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkCreateImage(MainRenderer->GetDevice(), &ImageCreateInfo, nullptr, &DepthStencilImage);

    VkMemoryRequirements ImageMemoryRequirements {};
    vkGetImageMemoryRequirements(MainRenderer->GetDevice(), DepthStencilImage, &ImageMemoryRequirements);

    uint32_t MemoryIndex = 0; // GetMemoryTypeIndex(&MainRenderer->GetPhysicalDeviceMemoryProperties(), &ImageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo MemoryAllocateInfo {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = ImageMemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = MemoryIndex;
    vkAllocateMemory(MainRenderer->GetDevice(), &MemoryAllocateInfo, nullptr, &DepthStencilImageMemory);
    vkBindImageMemory(MainRenderer->GetDevice(), DepthStencilImage, DepthStencilImageMemory, 0);

    VkImageViewCreateInfo ImageViewCreateInfo {};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.image = DepthStencilImage;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format = DepthStencilFormat;
    ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (IsStencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(MainRenderer->GetDevice(), &ImageViewCreateInfo, nullptr, &DepthStencilImageView);
}

void WindowOld::DestroyDepthStencilImage()
{
    vkDestroyImageView(MainRenderer->GetDevice(), DepthStencilImageView, nullptr);
    vkFreeMemory(MainRenderer->GetDevice(), DepthStencilImageMemory, nullptr);
    vkDestroyImage(MainRenderer->GetDevice(), DepthStencilImage, nullptr);
}

void WindowOld::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 2> Attachments {};

    Attachments[0].flags = 0;
    Attachments[0].format = DepthStencilFormat;
    Attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    Attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    Attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    Attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    Attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    Attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    Attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    Attachments[1].flags = 0;
    Attachments[1].format = SurfaceFormat.format;
    Attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    Attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    Attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    Attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    Attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference SubpassDepthStencilAttachment {};
    SubpassDepthStencilAttachment.attachment = 0;
    SubpassDepthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 1> SubpassColorAttachments;
    SubpassColorAttachments[0].attachment = 1;
    SubpassColorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::array<VkSubpassDescription, 1> Subpasses {};
    Subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpasses[0].colorAttachmentCount = SubpassColorAttachments.size();
    Subpasses[0].pColorAttachments = SubpassColorAttachments.data();
    Subpasses[0].pDepthStencilAttachment = &SubpassDepthStencilAttachment;

    VkRenderPassCreateInfo RenderPassCreateInfo {};
    RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassCreateInfo.attachmentCount = Attachments.size();
    RenderPassCreateInfo.pAttachments = Attachments.data();
    RenderPassCreateInfo.subpassCount = Subpasses.size();
    RenderPassCreateInfo.pSubpasses = Subpasses.data();

    (vkCreateRenderPass(MainRenderer->GetDevice(), &RenderPassCreateInfo, nullptr, &RenderPass));
}

void WindowOld::DestroyRenderPass()
{
    vkDestroyRenderPass(MainRenderer->GetDevice(), RenderPass, nullptr);
}

void WindowOld::CreateFramebuffer()
{
    Framebuffers.resize(SwapchainImageCount);
    for (uint32_t i = 0; i < SwapchainImageCount; ++i)
    {
        std::array<VkImageView, 2> Attachments {};
        Attachments[0] = DepthStencilImageView;
        Attachments[1] = SwapchainImageViews[i];

        VkFramebufferCreateInfo FramebufferCreateInfo = {};
        FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferCreateInfo.renderPass = RenderPass;
        FramebufferCreateInfo.attachmentCount = Attachments.size();
        FramebufferCreateInfo.pAttachments = Attachments.data();
        FramebufferCreateInfo.width = SurfaceSizeX;
        FramebufferCreateInfo.height = SurfaceSizeY;
        FramebufferCreateInfo.layers = 1;
        vkCreateFramebuffer(MainRenderer->GetDevice(), &FramebufferCreateInfo, nullptr, &Framebuffers[i]);
    }
}

void WindowOld::DestroyFramebuffer()
{
    for (auto & Buffer : Framebuffers)
    {
        vkDestroyFramebuffer(MainRenderer->GetDevice(), Buffer, nullptr);
    }
}

void WindowOld::CreateSynchronization()
{
    VkFenceCreateInfo FenceCreateInfo {};
    FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    vkCreateFence(MainRenderer->GetDevice(), &FenceCreateInfo, nullptr, &SwapchainImageAvailable);
}

void WindowOld::DestroySynchronization()
{
    vkDestroyFence(MainRenderer->GetDevice(), SwapchainImageAvailable, nullptr);
}
