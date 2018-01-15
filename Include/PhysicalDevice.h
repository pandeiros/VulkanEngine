/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include <vector>

/**
 * @file PhysicalDevice.h
 */

VULKAN_NS_BEGIN

/**
 * @class PhysicalDevice
 */
class PhysicalDevice : public VulkanClass
{
public:
    /**
     * Default constructor.
     */
    PhysicalDevice() = default;

    /**
     * Default destructor.
     */
    ~PhysicalDevice() = default;

    void Create(const VkPhysicalDevice physicalDevice);

    VkPhysicalDevice GetVkPhysicalDevice() const;

    VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;
    VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const;
    VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const;

    const std::vector<VkLayerProperties>& GetAvailableDeviceLayers() const;
    const std::vector<VkExtensionProperties>& GetAvailableDeviceExtensions() const;
    const std::vector<VkDisplayPropertiesKHR>& GetPhysicalDeviceDisplayProperties() const;

    uint32_t GetGraphicsQueueFamilyIndex() const;
    uint32_t GetPresentQueueFamilyIndex(VkSurfaceKHR surface);

    VkFormatProperties GetFormatProperties(VkFormat format) const;
    VkImageFormatProperties GetImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling,
        VkImageUsageFlags usage, VkImageCreateFlags flags) const;
    std::vector<VkSparseImageFormatProperties> GetSparseImageFormatProperties(VkFormat format, VkImageType type,
        VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling) const;

    void LogInfo();

    uint32_t GetMemoryTypeIndex(const VkMemoryRequirements* memoryRequirements, const VkMemoryPropertyFlags requiredProperties);

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    VkPhysicalDeviceFeatures supportedFeatures = {};

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    std::vector<VkLayerProperties> availableDeviceLayers;
    std::vector<VkExtensionProperties> availableDeviceExtensions;
    std::vector<VkDisplayPropertiesKHR> displayProperties;

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;
};

VULKAN_NS_END