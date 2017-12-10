/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Instance.h"
#include "Utils/Logger.h"
#include "AndroidUtils.h"

#include <sstream>

VULKAN_NS_USING;

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

    DebugTools::Verify(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

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
    DebugTools::Assert(width > 0 && height > 0, "Invalid window size.");
#elif _WIN32
    height = 1080;
    width = 1920;
#endif

    window.Create(this, { "Vulkan Engine", { (uint32_t)width, (uint32_t)height } });
}

void Instance::Destroy()
{
    window.Destroy();
    device.Destroy();
    DestroyDebug();

    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
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
    std::ostringstream oss;
    if (MsgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        oss << "INFO: ";
    }
    if (MsgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        oss << "WARNING: ";
    }
    if (MsgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        oss << "PERFORMANCE: ";
    }
    if (MsgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        oss << "ERROR: ";
    }
    if (MsgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        oss << "DEBUG: ";
    }

    oss << "@[" << LayerPrefix << "] " << Msg << "\n";
//    Logger::Log(oss.str());
    Logger::Log(oss.str().c_str());

#ifdef _WIN32
    if (MsgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        MessageBox(NULL, oss.str().c_str(), "Vulkan Error!", 0);
    }
#endif

    return VK_FALSE;
}

void Instance::SetupDebug()
{
    debugCallbackCreateInfo = {
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        nullptr,
        //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        //VK_DEBUG_REPORT_DEBUG_BIT_EXT |
        0,
        VulkanDebugCallback,
        nullptr
    };

    VULKAN_GET_INSTANCE_PROC_ADDR(instance, CreateDebugReportCallbackEXT);
    VULKAN_GET_INSTANCE_PROC_ADDR(instance, DestroyDebugReportCallbackEXT);

    // #TODO Remove after fixing VULKAN_GET_INSTANCE_PROC_ADDR in VulkanMisc)
    DebugTools::Assert(fvkCreateDebugReportCallbackEXT && fvkDestroyDebugReportCallbackEXT, "Cannot fetch debug function pointers.");

    DebugTools::Verify(fvkCreateDebugReportCallbackEXT(instance, &debugCallbackCreateInfo, nullptr, &debugReport));
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

void Renderer::SetupDebug()
{}
void Renderer::DestroyDebug()
{}

#endif // VULKAN_ENABLE_DEBUG