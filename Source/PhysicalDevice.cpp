/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "PhysicalDevice.h"

VULKAN_NS_USING;

void PhysicalDevice::Create(const VkPhysicalDevice physicalDevice)
{
    this->physicalDevice = physicalDevice;

    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
    queueFamilyProperties.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueFamilyProperties.data());

    bool bFamilyIndexFound = false;
    for (uint32_t i = 0; i < familyCount; ++i)
    {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamilyIndex = i;
            bFamilyIndexFound = true;
        }
    }

    DebugTools::Assert(bFamilyIndexFound, "Queue family supporting graphics not found.");

    uint32_t layerCount = 0;
    vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, nullptr);
    availableDeviceLayers.resize(layerCount);
    vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, availableDeviceLayers.data());

    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    availableDeviceExtensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data());
}

VkPhysicalDevice PhysicalDevice::GetVkPhysicalDevice() const
{
    return physicalDevice;
}

VkPhysicalDeviceProperties PhysicalDevice::GetPhysicalDeviceProperties() const
{
    return physicalDeviceProperties;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::GetPhysicalDeviceMemoryProperties() const
{
    return memoryProperties;
}

VkPhysicalDeviceFeatures PhysicalDevice::GetPhysicalDeviceFeatures() const
{
    return supportedFeatures;
}

const std::vector<VkExtensionProperties>& PhysicalDevice::GetAvailableDeviceExtensions() const
{
    return availableDeviceExtensions;
}

const std::vector<VkLayerProperties>& PhysicalDevice::GetAvailableDeviceLayers() const
{
    return availableDeviceLayers;
}

uint32_t PhysicalDevice::GetGraphicsFamilyIndex() const
{
    return graphicsFamilyIndex;
}

VkFormatProperties PhysicalDevice::GetFormatProperties(VkFormat format) const
{
    VkFormatProperties formatProperties;

    if (physicalDevice)
    {
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
    }

    return formatProperties;
}

VkImageFormatProperties PhysicalDevice::GetImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags) const
{
    VkImageFormatProperties imageFormatProperties;

    if (physicalDevice)
    {
        vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, &imageFormatProperties);
    }

    return imageFormatProperties;
}

std::vector<VkSparseImageFormatProperties> PhysicalDevice::GetSparseImageFormatProperties(VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling) const
{
    std::vector<VkSparseImageFormatProperties> sparseImageFormatProperties;

    uint32_t sparseImageFormatPropertiesCount = 0;
    vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, &sparseImageFormatPropertiesCount, nullptr);
    sparseImageFormatProperties.resize(sparseImageFormatPropertiesCount);
    vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, &sparseImageFormatPropertiesCount, sparseImageFormatProperties.data());

    return sparseImageFormatProperties;
}

void PhysicalDevice::LogInfo()
{
    Logger::Log(physicalDeviceProperties.deviceName);
    Logger::Log(physicalDeviceProperties.deviceID);

    Logger::Log("Device layers");
    for (VkLayerProperties& layer : availableDeviceLayers)
    {
        Logger::Log(layer.layerName);
    }

    Logger::Log("Device extensions");

    for (VkExtensionProperties& extension : availableDeviceExtensions)
    {
        Logger::Log(extension.extensionName);
    }
}
