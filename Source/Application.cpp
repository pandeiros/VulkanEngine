/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Application.h"
#include "Core.h"
#include "Instance.h"

VULKAN_NS_USING;

Application::Application(const char* applicationName, uint32_t applicationVersion, uint32_t apiVersion)
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

    Engine::InitStatic();
    Engine::GetEngine()->RegisterObject(this);

    instance = std::make_unique<Instance>();

    instance->Create(applicationInfo,
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

    Engine::GetEngine()->EnumeratePhysicalDevices(instance->GetVkInstance());

    instance->Init();

    Engine::GetEngine()->LogSystemInfo();
}

void Application::Destroy()
{
    Engine::GetEngine()->DestroyStatic(instance->GetDevice()->GetVkDevice());

    instance->Destroy();
}

//void Application::Create(const char* applicationName, uint32_t applicationVersion, uint32_t apiVersion)
//{
//    applicationInfo = {
//        VK_STRUCTURE_TYPE_APPLICATION_INFO,
//        nullptr,
//        applicationName,
//        applicationVersion,
//        VULKAN_ENGINE_NAME,
//        VULKAN_ENGINE_VERSION,
//        apiVersion
//    };
//}

Instance& Application::GetInstanceRef()
{
    return instance;
}
