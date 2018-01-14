/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
#include "Engine.h"
#include "Device.h"
#include "Window.h"

/**
 * @file Instance.h
 */

VULKAN_NS_BEGIN

/**
 * @class Instance
 */
class Instance : public VulkanClass
{
public:
    /**
     * Default constructor.
     */
    Instance() = default;

    /**
     * Default destructor.
     */
    ~Instance() = default;

    virtual void Init() override;
    virtual void Destroy() override;

    void Create(VkApplicationInfo applicationInfo, std::vector<const char*> instanceLayers, std::vector<const char*> instaceExtensions);

    void CreateDevice();
    void CreateAppWindow();

    VkInstance GetVkInstance();
    Device& GetDeviceRef();
    Window& GetWindowRef();

    //////////////////////////////////////////////////////////////////////////
    // Debug
    //////////////////////////////////////////////////////////////////////////

    void SetupDebug();
    void DestroyDebug();

private:
    VkInstance instance = VK_NULL_HANDLE;

    Device device;
    Window window;

    VkInstanceCreateInfo instanceCreateInfo = {};

    //////////////////////////////////////////////////////////////////////////
    // Debug
    //////////////////////////////////////////////////////////////////////////

    VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
    VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};

    PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;
};

VULKAN_NS_END