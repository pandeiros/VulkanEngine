/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"

#include <vector>

/**
 * @file PhysicalDevice.h
 */

VULKAN_NS_BEGIN

/**
 * @class VulkanEngineHeader
 */
class PhysicalDevice
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

    uint32_t GetGraphicsFamilyIndex() const;

    VkFormatProperties GetFormatProperties(VkFormat format)const;
    VkImageFormatProperties GetImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling,
        VkImageUsageFlags usage, VkImageCreateFlags flags) const;
    std::vector<VkSparseImageFormatProperties> GetSparseImageFormatProperties(VkFormat format, VkImageType type,
        VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling) const;

    void LogInfo();

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    VkPhysicalDeviceFeatures supportedFeatures = {};

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    std::vector<VkLayerProperties> availableDeviceLayers;
    std::vector<VkExtensionProperties> availableDeviceExtensions;

    uint32_t graphicsFamilyIndex = 0;
};

VULKAN_NS_END