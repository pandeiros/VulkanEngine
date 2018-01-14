/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Instance.h"
#include "Core.h"
#include "AndroidUtils.h"

#include <sstream>

VULKAN_NS_USING;

void Instance::Init()
{
    CreateDevice();
    CreateAppWindow();
}

void Instance::Destroy()
{
    window.Destroy();
    device.Destroy();
    DestroyDebug();

    if (instance)
    {
        vkDestroyInstance(instance, nullptr);
    }

    instance = VK_NULL_HANDLE;
}

void Instance::Create(VkApplicationInfo applicationInfo, std::vector<const char*> instanceLayers, std::vector<const char*> instaceExtensions)
{
    Engine* engine = Engine::GetEngine();

    if (engine)
    {
        engine->ValidateInstanceProperties(instanceLayers, instaceExtensions);
    }

    instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &applicationInfo,
        (uint32_t)instanceLayers.size(),
        instanceLayers.data(),
        (uint32_t)instaceExtensions.size(),
        instaceExtensions.data()
    };

    VK_VERIFY(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    SetupDebug();
}

void Instance::CreateDevice()
{
    Engine* engine = Engine::GetEngine();

    if (engine && engine->GetPhysicalDevices().size() > 0)
    {
        device.Create(engine->GetPhysicalDevices()[0],
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        },
        {});
    }
}

void Instance::CreateAppWindow()
{
    int32_t height = 0, width = 0;

#ifdef __ANDROID__
    AndroidGetWindowSize(width, height);
    VK_ASSERT(width > 0 && height > 0, "Invalid window size.");
#elif _WIN32
    height = 1080;
    width = 1920;
#endif

    window.Create(this, { "Vulkan Engine", { (uint32_t)width, (uint32_t)height } });
}

VkInstance Instance::GetVkInstance()
{
    return instance;
}

Device& Instance::GetDeviceRef()
{
    return device;
}

Window& Instance::GetWindowRef()
{
    return window;
}

#if VULKAN_ENABLE_DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(VkDebugReportFlagsEXT MsgFlags, VkDebugReportObjectTypeEXT ObjectType, uint64_t SourceObject,
    size_t Location, int32_t MsgCode, const char* LayerPrefix, const char* Msg, void* UserData)
{
    VULKAN_LOGGER(LogVulkan.GetCategoryName(), Logger::GetVerbosity(MsgFlags), "%s", Msg);

    return VK_FALSE;
}

void Instance::SetupDebug()
{
    debugCallbackCreateInfo = {
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        nullptr,
        DebugTools::GetVulkanDebugFlagsEnabled(),
        VulkanDebugCallback,
        nullptr
    };

    VULKAN_GET_INSTANCE_PROC_ADDR(instance, CreateDebugReportCallbackEXT);
    VULKAN_GET_INSTANCE_PROC_ADDR(instance, DestroyDebugReportCallbackEXT);

    // #TODO Remove after fixing VULKAN_GET_INSTANCE_PROC_ADDR in VulkanMisc)
    //VK_ASSERT(fvkCreateDebugReportCallbackEXT && fvkDestroyDebugReportCallbackEXT, "Cannot fetch debug function pointers.");

    VK_VERIFY(fvkCreateDebugReportCallbackEXT(instance, &debugCallbackCreateInfo, nullptr, &debugReport));
}

void Instance::DestroyDebug()
{
    if (instance && debugReport)
    {
        fvkDestroyDebugReportCallbackEXT(instance, debugReport, nullptr);
    }

    debugReport = VK_NULL_HANDLE;
}

#else

void Instance::SetupDebug()
{}
void Instance::DestroyDebug()
{}

#endif // VULKAN_ENABLE_DEBUG