/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#define VULKAN_ENGINE_VERSION VK_MAKE_VERSION(0, 0, 2)
#define VULKAN_ENGINE_NAME "Vulkan Engine"

#include "Utils/Logger.h"
#include "PhysicalDevice.h"

#include <vector>

/**
 * @file Engine.h
 */

VULKAN_NS_BEGIN

/**
 * @struct LayerProperties
 */
struct LayerProperties
{
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

/**
 * @class Engine
 */
class Engine
{
public:
    static void RegisterGlobalEngine(Engine* engine);
    static Engine* GetEngine();

    void LogSystemInfo();

    void InitInstanceProperties();
    void ValidateInstanceProperties(std::vector<const char*>& instanceLayers, std::vector<const char*>& instaceExtensions);

    void EnumeratePhysicalDevices(VkInstance instance);
    std::vector<PhysicalDevice>& GetPhysicalDevices();

private:
    VkResult EnumerateInstanceLayers();
    VkResult EnumerateInstanceExtensions(LayerProperties& layerProperties);

    std::vector<LayerProperties> instanceProperties;
    std::vector<VkExtensionProperties> globalInstanceExtensions;

    //std::vector<VkLayerProperties,

    std::vector<PhysicalDevice> physicalDevices;

    static Engine* engine;
};

VULKAN_NS_END