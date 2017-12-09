/**
* Vulkan Engine
*
* Copyright (C) 2016-2017 Pawel Kaczynski
*/

#include "Engine.h"

#include <cstdlib>
#include <algorithm>

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

    Logger::Log("\nPhysical devices:");
    for (PhysicalDevice& physicalDevice : physicalDevices)
    {
        physicalDevice.LogInfo();
    }
}

void Engine::InitInstanceProperties()
{
    DebugTools::Verify(EnumerateInstanceLayers());

    // #TODO init global extensions!!!
    uint32_t globalExtensionCount;
}

void Engine::ValidateInstanceProperties(std::vector<const char*> instanceLayers, std::vector<const char*> instaceExtensions)
{
    if (instanceProperties.size() == 0)
    {
        instanceLayers.clear();
        instaceExtensions.clear();
    }
    else
    {
        for (unsigned int layerIndex = instanceLayers.size() - 1; layerIndex >= 0; --layerIndex) // const char* layer : instanceLayers)
        {
            const char* layer = instanceLayers[layerIndex];

            std::vector<LayerProperties>::iterator layerIterator = std::find_if(instanceProperties.begin(), instanceProperties.end(), [&layer](const LayerProperties& layerProperties)
            {
                return layerProperties.properties.layerName == layer;
            });

            if (layerIterator != instanceProperties.end())
            {
                for (unsigned int extensionIndex = instaceExtensions.size() - 1; extensionIndex >= 0; --extensionIndex)
                {
                    const char* extension = instaceExtensions[extensionIndex];

                    std::vector<VkExtensionProperties>::iterator extensionIterator = std::find_if(layerIterator->extensions.begin(), layerIterator->extensions.end(), [&extension](const VkExtensionProperties& extensionProperties)
                    {
                        return extensionProperties.extensionName == extension;
                    });

                    if (extensionIterator == layerIterator->extensions.end())
                    {
                        instaceExtensions.pop_back();
                    }
                }
            }
            else
            {
                instanceLayers.pop_back();
            }
        }
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

    /*
    * Now gather the extension list for each instance layer.
    */
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
    //VkExtensionProperties* extensions;
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
