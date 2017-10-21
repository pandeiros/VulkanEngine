/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#define VULKAN_ENGINE_VERSION VK_MAKE_VERSION(1, 0, 2)
#define VULKAN_ENGINE_NAME "Vulkan Engine"

#include "Tools/Logger.h"
#include "PhysicalDevice.h"

#include <vector>

/**
 * @file Engine.h
 */
VULKAN_NS_BEGIN

    /**
     * @class Engine
     */
    class Engine
    {
    public:
        static void RegisterGlobalEngine(Engine* engine);
        static Engine* GetEngine();

        void LogSystemInfo();

        void EnumerateInstanceProperties();
        void EnumeratePhysicalDevices(VkInstance instance);

        std::vector<PhysicalDevice>& GetPhysicalDevices();

    private:
        std::vector<VkLayerProperties> instanceLayers;
        std::vector<VkExtensionProperties> instanceExtensions;

        std::vector<PhysicalDevice> physicalDevices;

        static Engine* engine;
    };

VULKAN_NS_END