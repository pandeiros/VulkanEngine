/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "PhysicalDevice.h"
#include "Core.h"

VULKAN_NS_USING;

VULKAN_DECLARE_LOG_CATEGORY_STATIC(LogPhysicalDevice);

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

    for (uint32_t i = 0; i < familyCount; ++i)
    {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            if (graphicsQueueFamilyIndex == UINT32_MAX)
            {
                graphicsQueueFamilyIndex = i;
            }

            break;
        }
    }

    VK_ASSERT(graphicsQueueFamilyIndex != UINT32_MAX, "Queue family supporting graphics not found.");

    uint32_t layerCount = 0;
    vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, nullptr);
    availableDeviceLayers.resize(layerCount);
    vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, availableDeviceLayers.data());

    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    availableDeviceExtensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data());

    //uint32_t displayPropertiesCount = 0;
    //vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &displayPropertiesCount, nullptr);
    //displayProperties.resize(displayPropertiesCount);
    //vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &displayPropertiesCount, displayProperties.data());
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

const std::vector<VkDisplayPropertiesKHR>& PhysicalDevice::GetPhysicalDeviceDisplayProperties() const
{
    return displayProperties;
}

const std::vector<VkLayerProperties>& PhysicalDevice::GetAvailableDeviceLayers() const
{
    return availableDeviceLayers;
}

uint32_t PhysicalDevice::GetGraphicsQueueFamilyIndex() const
{
    return graphicsQueueFamilyIndex;
}

uint32_t PhysicalDevice::GetPresentQueueFamilyIndex(VkSurfaceKHR surface)
{
    VkBool32 bPresentSupported = false;
    uint32_t tempGraphicsIndex = UINT32_MAX;

    for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
    {
        if (queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &bPresentSupported);

            if (bPresentSupported == VK_TRUE)
            {
                tempGraphicsIndex = index;
                presentQueueFamilyIndex = index;

                break;
            }
        }
    }

    VK_ASSERT(presentQueueFamilyIndex != UINT32_MAX, "Present queue family index not found!");

    if (tempGraphicsIndex != graphicsQueueFamilyIndex)
    {
        graphicsQueueFamilyIndex = tempGraphicsIndex;
        SetDirty(true);
    }

    return presentQueueFamilyIndex;
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
    VK_LOG(LogPhysicalDevice, Debug, "Physical device:");
    VK_LOG(LogPhysicalDevice, Debug, "%s", physicalDeviceProperties.deviceName);
    VK_LOG(LogPhysicalDevice, Debug, "%u", physicalDeviceProperties.deviceID);

    VK_LOG(LogPhysicalDevice, Debug, "Graphics queue family index: %u", graphicsQueueFamilyIndex);
    VK_LOG(LogPhysicalDevice, Debug, "Present queue family index: %u", presentQueueFamilyIndex);

    VK_LOG(LogPhysicalDevice, Debug, "Device layers:");
    for (VkLayerProperties& layer : availableDeviceLayers)
    {
        VK_LOG(LogPhysicalDevice, Debug, "  %s", layer.layerName);
    }

    VK_LOG(LogPhysicalDevice, Debug, "Device extensions:");
    for (VkExtensionProperties& extension : availableDeviceExtensions)
    {
        VK_LOG(LogPhysicalDevice, Debug, "  %s", extension.extensionName);
    }

    VK_LOG(LogPhysicalDevice, Debug, "Device display properties:");
    for (VkDisplayPropertiesKHR& property : displayProperties)
    {
        VK_LOG(LogPhysicalDevice, Debug, "  %s", property.displayName);
    }
}

uint32_t PhysicalDevice::GetMemoryTypeIndex(const VkMemoryRequirements* MemoryRequirements, const VkMemoryPropertyFlags RequiredProperties)
{
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if (MemoryRequirements->memoryTypeBits & (1 << i))
        {
            if ((memoryProperties.memoryTypes[i].propertyFlags & RequiredProperties) == RequiredProperties)
            {
                return i;
            }
        }
    }

    VK_ASSERT(0, "Could not find property memory type.");

    return UINT32_MAX;

}
