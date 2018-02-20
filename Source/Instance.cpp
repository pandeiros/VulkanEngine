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

Instance::Instance(VkApplicationInfo applicationInfo, std::vector<const char*> instanceLayers,
    std::vector<const char*> instaceExtensions)
{
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

Instance::~Instance()
{
    Window* pWindow = window.release();
    if (pWindow)
    {
        delete pWindow;
    }

    VK_ASSERT(device.use_count() == 1, "Device shared_ptr reference count should equal 1 when destroying instance!");
    device.reset();

    DestroyDebug();

    if (instance)
    {
        vkDestroyInstance(instance, nullptr);
    }

    instance = VK_NULL_HANDLE;
}

void Instance::InitDeviceAndWindow(PhysicalDevice* physicalDevice)
{
    // Device
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkPhysicalDeviceFeatures requiredFeatures = {};

#ifdef VULKAN_VR_MODE_VIEWPORTS
    requiredFeatures.multiViewport = VK_TRUE;
#endif

#ifndef __ANDROID__
    requiredFeatures.tessellationShader = VK_TRUE;
#endif

    requiredFeatures.fillModeNonSolid = VK_TRUE;

    physicalDevice->CheckRequiredFeatures(requiredFeatures);

    device = std::shared_ptr<Device>(new Device(physicalDevice,
        deviceExtensions, requiredFeatures));

    // Window
    int32_t height = 0, width = 0;

#ifdef __ANDROID__
    AndroidGetWindowSize(width, height);
    VK_ASSERT(width > 0 && height > 0, "Invalid window size.");
#elif _WIN32
    height = 768;
    width = 1366;
#endif

    window.reset(new Window(device, instance, { "Vulkan Engine", { (uint32_t)width, (uint32_t)height } }));

}

VkInstance Instance::GetVkInstance()
{
    return instance;
}

std::shared_ptr<Device> Instance::GetDevicePtr()
{
    return std::shared_ptr<Device>(device);
}

Device* Instance::GetDevice()
{
    return device.get();
}

Window* Instance::GetWindow()
{
    return window.get();
}

//Window& Instance::GetWindowRef()
//{
//    return window;
//}

#if VULKAN_ENABLE_DEBUG

static VKAPI_ATTR VkBool32 VKAPI_CALL
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
        (PFN_vkDebugReportCallbackEXT)VulkanDebugCallback,
        nullptr
    };

    VULKAN_GET_INSTANCE_PROC_ADDR(instance, CreateDebugReportCallbackEXT);
    VULKAN_GET_INSTANCE_PROC_ADDR(instance, DestroyDebugReportCallbackEXT);

    VK_VERIFY(vkCreateDebugReportCallbackEXT(instance, &debugCallbackCreateInfo, nullptr, &debugReport));
}

void Instance::DestroyDebug()
{
    if (instance && debugReport)
    {
        vkDestroyDebugReportCallbackEXT(instance, debugReport, nullptr);
    }

    debugReport = VK_NULL_HANDLE;
}

#else

void Instance::SetupDebug()
{}
void Instance::DestroyDebug()
{}

#endif // VULKAN_ENABLE_DEBUG