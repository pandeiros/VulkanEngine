/**
* Vulkan Engine
*
* Copyright (C) 2016-2017 Pawel Kaczynski
*/

#include "Engine.h"

#include <cstdlib>
#include <algorithm>

#include "Utils/Math.h"

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
    if (instanceProperties.size())
    {
        InitInstanceProperties();
    }

    Logger::Log("Instance properties:");
    for (LayerProperties& instanceProperty : instanceProperties)
    {
        Logger::Log(instanceProperty.properties.layerName);
        for (VkExtensionProperties& extensionProperty : instanceProperty.extensions)
        {
            Logger::Log(extensionProperty.extensionName);
        }
    }

    Logger::Log("\nGlobal extensions:");
    for (VkExtensionProperties extension : globalInstanceExtensions)
    {
        Logger::Log(extension.extensionName);
    }

    Logger::Log("\nPhysical devices:");
    for (PhysicalDevice& physicalDevice : physicalDevices)
    {
        physicalDevice.LogInfo();
    }
}

void Engine::InitInstanceProperties()
{
    DebugTools::Verify(EnumerateInstanceLayers());

    uint32_t globalExtensionCount = 0;
    VkResult result;
    do
    {
        DebugTools::Verify(vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionCount, nullptr));

        if (globalExtensionCount == 0)
        {
            return ;
        }

        globalInstanceExtensions.resize(globalExtensionCount);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionCount, globalInstanceExtensions.data());
    } while (result == VK_INCOMPLETE);

}

void Engine::ValidateInstanceProperties(std::vector<const char*>& instanceLayers, std::vector<const char*>& instanceExtensions)
{
    if (instanceProperties.size() == 0)
    {
        instanceLayers.clear();
    }
    else
    {
        std::vector<const char*> instanceLayersCopy = instanceLayers;

        // First we check if specified layers are valid.
        std::vector<LayerProperties> instancePropertiesCopy = instanceProperties;
        instanceLayers.erase(std::remove_if(instanceLayers.begin(), instanceLayers.end(), [&instancePropertiesCopy](const char* layer)
        {
            Logger::Log((std::string(" >>> ") + std::string(layer)).c_str());
            return std::find_if(instancePropertiesCopy.begin(), instancePropertiesCopy.end(), [&layer](const LayerProperties& layerProperties)
            {
                return layerProperties.properties.layerName == layer;
            }) != instancePropertiesCopy.end();
        }), instanceLayers.end());

        // #TODO Make this as a verbose log.
        //std::vector<const char*> instanceLayersDiff = Math::Diff<const char*>(instanceLayersCopy, instanceLayers);
        //Logger::Log("  Instance layers diff:");
        //Math::Print(instanceLayersDiff);
    }

    if (globalInstanceExtensions.size() == 0)
    {
        instanceExtensions.clear();
    }
    else
    {
        std::vector<const char*> instanceExtensionsCopy = instanceExtensions;

        // We assume, that all layer-specific extensions are present in global extensions container.
        std::vector<VkExtensionProperties> globalInstanceExtensionsCopy = globalInstanceExtensions;
        instanceExtensions.erase(std::remove_if(instanceExtensions.begin(), instanceExtensions.end(), [&globalInstanceExtensionsCopy](const char* extension)
        {
            return (std::find_if(globalInstanceExtensionsCopy.begin(), globalInstanceExtensionsCopy.end(), [&extension](const VkExtensionProperties& extensionProperties)
            {
                return extensionProperties.extensionName == extension;
            }) != globalInstanceExtensionsCopy.end());
        }), instanceExtensions.end());

        // #TODO Make this as a verbose log.
        //std::vector<const char*> instanceExtensionsDiff = Math::Diff<const char*>(instanceExtensionsCopy, instanceExtensions);
        //Logger::Log("  Instance extensions diff:");
        //Math::Print(instanceExtensionsDiff);
    }
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

VkResult Engine::EnumerateInstanceLayers()
{
    VkLayerProperties* properties = nullptr;
    VkResult result;
    uint32_t layerCount = 0;

    // #TODO Edit.
    /*
    * It's possible, though very rare, that the number of
    * instance layers could change. For example, installing something
    * could include new layers that the loader would pick up
    * between the initial query for the count and the
    * request for VkLayerProperties. The loader indicates that
    * by returning a VK_INCOMPLETE status and will update the
    * the count parameter.
    * The count parameter will be updated with the number of
    * entries loaded into the data pointer - in case the number
    * of layers went down or is smaller than the size given.
    */
    do
    {
        result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result)
        {
            return result;
        }

        if (layerCount == 0)
        {
            return VK_SUCCESS;
        }

        properties = (VkLayerProperties*)realloc(properties, layerCount * sizeof(VkLayerProperties));
        result = vkEnumerateInstanceLayerProperties(&layerCount, properties);
    } while (result == VK_INCOMPLETE);

    /** Now gather the extension list for each instance layer. */
    for (uint32_t i = 0; i < layerCount; i++)
    {
        LayerProperties layerProperties;
        layerProperties.properties = properties[i];

        result = EnumerateInstanceExtensions(layerProperties);
        if (result)
        {
            return result;
        }

        instanceProperties.push_back(layerProperties);
    }

    free(properties);

    return result;
}

VkResult Engine::EnumerateInstanceExtensions(LayerProperties& layerProperties)
{
    uint32_t extensionCount;
    VkResult result;
    char* layerName = nullptr;

    layerName = layerProperties.properties.layerName;

    do
    {
        result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, NULL);
        if (result)
        {
            return result;
        }

        if (extensionCount == 0)
        {
            return VK_SUCCESS;
        }

        layerProperties.extensions.resize(extensionCount);
        result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, layerProperties.extensions.data());
    } while (result == VK_INCOMPLETE);

    return result;
}
