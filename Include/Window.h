/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
#include "PhysicalDevice.h"
#include "Image.h"
#include "ImageView.h"

/**
 * @file Window.h
 */

VULKAN_NS_BEGIN

class Instance;

struct WindowCreateInfo
{
    std::string windowName;
    VkExtent2D surfaceSize;
};

/**
 * @class Window
 */
class Window
{
public:
    /**
     * Default constructor.
     */
    Window() = default;

    /**
     * Default destructor.
     */
    ~Window() = default;

    void Create(Instance* instance, const WindowCreateInfo& windowCreateInfo);
    void Destroy();

    bool Update();
    void Close();

    void BeginRender();
    void EndRender(std::vector<VkSemaphore> WaitSemaphores);

    VkExtent2D GetSurfaceSize();
    VkRenderPass GetRenderPass();
    VkFramebuffer GetActiveFramebuffer();

private:
    WindowCreateInfo windowCreateInfo = {};
    Instance* cachedInstance = VK_NULL_HANDLE;

    bool bIsValid = true;

    //////////////////////////////////////////////////////////////////////////
    // Surface
    //////////////////////////////////////////////////////////////////////////

    void CreateSurface();
    void DestroySurface();

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    VkSurfaceFormatKHR surfaceFormat = {};
    uint32_t presentQueueFamilyIndex = UINT32_MAX;

    //////////////////////////////////////////////////////////////////////////
    // Swapchain
    //////////////////////////////////////////////////////////////////////////

    // #TODO Create Swapchain class.

    void CreateSwapchain();
    void DestroySwapchain();

    void CreateSwapchainImages();
    void DestroySwapchainImages();

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    uint32_t swapchainImageCount = 2;
    uint32_t activeSwapchainImageID = UINT32_MAX;

    std::vector<VkImage> swapchainImages;
    std::vector<ImageView> swapchainImageViews;

    //////////////////////////////////////////////////////////////////////////
    // Depth-stencil images
    //////////////////////////////////////////////////////////////////////////

    void CreateDepthStencilImage();
    void DestroyDepthStencilImage();

    Image depthStencilImage;
    ImageView depthStencilImageView;
    VkDeviceMemory depthStencilImageMemory = VK_NULL_HANDLE;
    //VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
    bool bStencilAvailable = false;

    //////////////////////////////////////////////////////////////////////////
    // Render pass
    //////////////////////////////////////////////////////////////////////////

    // #TODO Create RenderPass class.

    void CreateRenderPass();
    void DestroyRenderPass();

    VkRenderPass renderPass = VK_NULL_HANDLE;

    //////////////////////////////////////////////////////////////////////////
    // Framebuffer
    //////////////////////////////////////////////////////////////////////////

    void CreateFramebuffer();
    void DestroyFramebuffer();

    std::vector<VkFramebuffer> framebuffers;

    //////////////////////////////////////////////////////////////////////////
    // Synchronization
    //////////////////////////////////////////////////////////////////////////

    void CreateSynchronization();
    void DestroySynchronization();

    VkFence fenceSwapchainImageAvailable = VK_NULL_HANDLE;

    //////////////////////////////////////////////////////////////////////////
    // OS specific methods
    //////////////////////////////////////////////////////////////////////////

    void CreateOSWindow();
    void CreateOSSurface();
    void UpdateOSWindow();
    void DestroyOSWindow();

    //////////////////////////////////////////////////////////////////////////
    // Win32 members
    //////////////////////////////////////////////////////////////////////////

#if _WIN32
    HINSTANCE win32Instance = NULL;
    HWND win32Window = NULL;
    std::string win32ClassName;
    static uint64_t win32ClassIdCounter;
#elif __ANDROID__
    PFN_vkCreateAndroidSurfaceKHR fvkCreateAndroidSurfaceKHR;
#endif
};

VULKAN_NS_END