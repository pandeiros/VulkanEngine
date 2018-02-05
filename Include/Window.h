/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Device.h"
#include "Image.h"
#include "ImageView.h"
#include "Memory.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "Swapchain.h"

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
class Window : public VulkanClass
{
public:
    Window(DevicePtr device, VkInstance instance, const WindowCreateInfo& windowCreateInfo);

    ~Window();

    //void Init() override;
    //void Create(Instance* instance, const WindowCreateInfo& windowCreateInfo);
    //void Destroy();

    bool Update();
    void Close();

    void BeginRender();
    // #TODO Maybe rename this to Present or sth
    void EndRender(std::vector<VkSemaphore> WaitSemaphores, std::vector<VkFence> waitFences);

    float GetAspectRatio();
    VkExtent2D GetSurfaceSize();
    VkRenderPass GetRenderPass();
    VkFramebuffer GetActiveFramebuffer();

    // #TODO Clean this up
    VkSemaphore GetSemaphore()
    {
        return semaphoreImageAcquired;
    }
    VkFence GetFence()
    {
        return fenceDraw;
    }

private:
    WindowCreateInfo windowCreateInfo = {};
    VkInstance instance = VK_NULL_HANDLE;

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

    void CreateSwapchain();
    void DestroySwapchain();

    void CreateSwapchainImages();
    void DestroySwapchainImages();

    Swapchain swapchain;
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
    Memory depthStencilImageMemory;

    bool bStencilAvailable = false;

    //////////////////////////////////////////////////////////////////////////
    // Render pass
    //////////////////////////////////////////////////////////////////////////

    void CreateRenderPass();
    void DestroyRenderPass();

    RenderPass renderPass;

    //////////////////////////////////////////////////////////////////////////
    // Framebuffer
    //////////////////////////////////////////////////////////////////////////

    void CreateFramebuffer();
    void DestroyFramebuffer();

    std::vector<Framebuffer> framebuffers;

    //////////////////////////////////////////////////////////////////////////
    // Synchronization
    //////////////////////////////////////////////////////////////////////////

    void CreateSynchronization();
    void DestroySynchronization();

    VkFence fenceDraw = VK_NULL_HANDLE;
    //VkFence fenceSwapchainImageAvailable = VK_NULL_HANDLE;
    VkSemaphore semaphoreImageAcquired = VK_NULL_HANDLE;

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
    //PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif
};

VULKAN_NS_END