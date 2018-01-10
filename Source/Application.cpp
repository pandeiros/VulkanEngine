/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Application.h"
#include "Core.h"
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
#ifdef __ANDROID__
    VK_ASSERT(InitVulkan() != 0, "Initialization of Vulkan APIs failed!");
#endif

    Engine::RegisterGlobalEngine(&engine);

    // Some debug stuff.
    engine.InitInstanceProperties();

    instance.Create(applicationInfo,
    {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_standard_validation"
    },
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VULKAN_PLATFORM_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    });

    engine.EnumeratePhysicalDevices(instance.GetVkInstance());

    instance.CreateDevice();

    instance.CreateAppWindow();

    engine.LogSystemInfo();
}

void Application::Destroy()
{
    instance.Destroy();
}

Instance& Application::GetInstanceRef()
{
    return instance;
}
