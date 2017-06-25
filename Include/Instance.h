/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"
#include "Engine.h"
#include "Device.h"

/**
 * @file Instance.h
 */

VULKAN_NS_BEGIN

/**
 * @class Instance
 */
class Instance
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

    void Create(VkApplicationInfo applicationInfo, std::vector<const char*> instanceLayers,
        std::vector<const char*> instaceExtensions);
    void CreateDevice();

    void Destroy();

    inline VkInstance GetVkInstance() { return instance; }

    //////////////////////////////////////////////////////////////////////////
    // Debug
    //////////////////////////////////////////////////////////////////////////

    void SetupDebug();
    void DestroyDebug();

private:
    VkInstance instance = VK_NULL_HANDLE;

    Device device;

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