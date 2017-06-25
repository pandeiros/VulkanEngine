/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Application.h"
#include "Instance.h"

VULKAN_NS_USING;

void Application::Create(const char* applicationName, uint32_t applicationVersion, uint32_t apiVersion)
{
    applicationInfo = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        applicationName,
        applicationVersion,
        VULKAN_ENGINE_NAME,
        VULKAN_ENGINE_VERSION,
        apiVersion
    };
}

void Application::Init()
{
    Engine::RegisterGlobalEngine(&engine);

    // Some debug stuff.
    engine.EnumerateInstanceProperties();

    instance.Create(applicationInfo,
    {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_swapchain"
    },
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VULKAN_PLATFORM_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    });

    engine.EnumeratePhysicalDevices(instance.GetVkInstance());
    engine.LogSystemInfo();

    instance.CreateDevice();
}

void Application::Destroy()
{
    instance.Destroy();
}
