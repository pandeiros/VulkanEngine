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

Application::~Application()
{
    //Engine::GetEngine()->DestroyStatic();

    engine.release();
    instance->Destroy();
}

void Application::Init()
{
#ifdef __ANDROID__
    VK_ASSERT(InitVulkan() != 0, "Initialization of Vulkan APIs failed!");
#endif

    //Engine::InitStatic();
    //Engine::GetEngine()->RegisterObject(this);

    engine.reset(new Engine(60, false));

    std::vector<const char*> instanceLayers = {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_standard_validation"
    };

    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VULKAN_PLATFORM_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };

    engine->ValidateInstanceProperties(instanceLayers, instanceExtensions);

    instance.reset(new Instance(applicationInfo, instanceLayers, instanceExtensions));

    //instance->Create(applicationInfo,
    //{
    //    "VK_LAYER_GOOGLE_threading",
    //    "VK_LAYER_LUNARG_object_tracker",
    //    "VK_LAYER_LUNARG_parameter_validation",
    //    "VK_LAYER_LUNARG_standard_validation"
    //},
    //{
    //    VK_KHR_SURFACE_EXTENSION_NAME,
    //    VULKAN_PLATFORM_SURFACE_EXTENSION_NAME,
    //    VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    //});

    engine->InitPhysicalDevices(instance->GetVkInstance());

    //engine->Init(instance->GetDevicePtr(), instance->GetVkInstance());

    //Engine::GetEngine()->EnumeratePhysicalDevices(instance->GetVkInstance());

    instance->InitDeviceAndWindow(engine->GetPhysicalDevice(0));

    engine->Init(instance->GetDevicePtr());

    engine->LogSystemInfo();
    //Engine::GetEngine()->LogSystemInfo();
}

//void Application::Destroy()
//{
//    Engine::GetEngine()->DestroyStatic(instance->GetDevice()->GetVkDevice());
//
//    instance->Destroy();
//}

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

Instance* Application::GetInstance()
{
    return instance.get();
}

Engine* Application::GetEngine()
{
    return engine.get();
}
