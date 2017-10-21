/*
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
#include <vector>

/**
 * @file Window.h
 */

VULKAN_NS_BEGIN

class Renderer;

/*
 * @class Window
 */
class WindowOld
{
public:
    /**
     * Default constructor.
     */
    WindowOld(Renderer * Renderer, uint32_t SizeX, uint32_t SizeY, std::string Name);

    /**
     * Default destructor.
     */
    ~WindowOld();

    void Close();
    bool Update();

    void BeginRender();
    void EndRender(std::vector<VkSemaphore> WaitSemaphores);

    VkRenderPass GetRenderPass();
    VkFramebuffer GetActiveFramebuffer();
    VkExtent2D GetSurfaceSize();

private:
    //void CreateOSWindow();
    //void DestroyOSWindow();
    //void UpdateOSWindow();
    //void CreateOSSurface();

    void CreateSurface();
    void DestroySurface();

    void CreateSwapchain();
    void DestroySwapchain();

    void CreateSwapchainImages();
    void DestroySwapchainImages();

    void CreateDepthStencilImage();
    void DestroyDepthStencilImage();

    void CreateRenderPass();
    void DestroyRenderPass();

    void CreateFramebuffer();
    void DestroyFramebuffer();

    void CreateSynchronization();
    void DestroySynchronization();

    Renderer* MainRenderer = nullptr;

    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    VkRenderPass RenderPass = VK_NULL_HANDLE;

    uint32_t SurfaceSizeX = 512;
    uint32_t SurfaceSizeY = 512;
    std::string WindowName;
    uint32_t SwapchainImageCount = 2;
    uint32_t ActiveSwapchainImageID = UINT32_MAX;

    VkFence SwapchainImageAvailable = VK_NULL_HANDLE;

    VkSurfaceFormatKHR SurfaceFormat = {};
    VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};

    VkImage DepthStencilImage = VK_NULL_HANDLE;
    VkImageView DepthStencilImageView = VK_NULL_HANDLE;
    VkDeviceMemory DepthStencilImageMemory = VK_NULL_HANDLE;
    VkFormat DepthStencilFormat = VK_FORMAT_UNDEFINED;
    bool IsStencilAvailable = false;

    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;
    std::vector<VkFramebuffer> Framebuffers;

    bool IsValid = true;

#if VK_USE_PLATFORM_WIN32_KHR
    HINSTANCE Win32Instance = NULL;
    HWND Win32Window = NULL;
    std::string Win32ClassName;
    static uint64_t Win32ClassIdCounter;
#endif
};

VULKAN_NS_END