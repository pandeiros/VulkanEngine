/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"
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
    Instance(VkApplicationInfo applicationInfo, std::vector<const char*> instanceLayers,
        std::vector<const char*> instaceExtensions);

    ~Instance();

    void InitDeviceAndWindow(PhysicalDevice* physicalDevice);

    VkInstance GetVkInstance();

    std::shared_ptr<Device> GetDevicePtr();
    Device* GetDevice();
    Window* GetWindow();

private:
    VkInstance instance = VK_NULL_HANDLE;

    std::shared_ptr<Device> device;
    std::unique_ptr<Window> window;

    VkInstanceCreateInfo instanceCreateInfo = {};

    //////////////////////////////////////////////////////////////////////////
    // Debug
    //////////////////////////////////////////////////////////////////////////

    void SetupDebug();
    void DestroyDebug();

    VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
    VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {};

#ifndef __ANDROID__
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
#endif
};

VULKAN_NS_END