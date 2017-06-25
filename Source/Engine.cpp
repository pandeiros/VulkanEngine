/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Engine.h"

VULKAN_NS_USING;

Engine* Engine::engine = nullptr;

void Engine::RegisterGlobalEngine(Engine* engine)
{
    Engine::engine = engine;
}

Engine* Engine::GetEngine()
{
    return engine;
}

void Engine::LogSystemInfo()
{
    if (instanceLayers.size() == 0 && instanceExtensions.size() == 0)
    {
        EnumerateInstanceProperties();
    }

    Logger::Log("Instance layers");
    for (VkLayerProperties& layer : instanceLayers)
    {
        Logger::Log(layer.layerName);
    }

    Logger::Log("\nInstance extensions");
    for (VkExtensionProperties& extension : instanceExtensions)
    {
        Logger::Log(extension.extensionName);
    }

    Logger::Log("\nPhysical devices");
    for (PhysicalDevice& physicalDevice : physicalDevices)
    {
        physicalDevice.LogInfo();
    }
}

void Engine::EnumerateInstanceProperties()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    instanceLayers.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    instanceExtensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());
}

void Engine::EnumeratePhysicalDevices(VkInstance instance)
{
    std::vector<VkPhysicalDevice> vkPhysicalDevices;
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);
    vkPhysicalDevices.resize(physicalDevicesCount);
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, vkPhysicalDevices.data());

    for (VkPhysicalDevice vkPhysicalDevice : vkPhysicalDevices)
    {
        PhysicalDevice physicalDevice;
        physicalDevice.Create(vkPhysicalDevice);

        physicalDevices.push_back(physicalDevice);
    }
}

std::vector<PhysicalDevice>& Engine::GetPhysicalDevices()
{
    return physicalDevices;
}