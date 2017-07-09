/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

/**
 * @file Renderer.h
 */

#include <vector>
#include "VulkanConfig.h"

VULKAN_NS_BEGIN

class WindowOld;

/*
 * @class Renderer
 */
class Renderer
{
public:
    /**
     * Default constructor.
     */
    Renderer();

    /**
     * Default destructor.
     */
    ~Renderer();

    WindowOld * OpenWindowOld(uint32_t SizeX, uint32_t SizeY, std::string Name);
    bool Run();

    const VkInstance GetInstance() const;
    const VkPhysicalDevice GetPhysicalDevice() const;
    const VkDevice GetDevice() const;
    const VkQueue GetQueue() const;
    const uint32_t GetGraphicsFamilyIndex() const;
    const VkPhysicalDeviceProperties & GetPhysicalDeviceProperties() const;
    const VkPhysicalDeviceMemoryProperties & GetPhysicalDeviceMemoryProperties() const;

private:
    void SetupLayerExtensions();

    void InitInstance();

    void DestroyInstance();

    void InitDevice();

    void DestroyDevice();

    void SetupDebug();

    void InitDebug();

    void DestroyDebug();

    VkInstance Instance = VK_NULL_HANDLE;
    VkPhysicalDevice GPU = VK_NULL_HANDLE;
    VkDevice Device = VK_NULL_HANDLE;
    VkQueue Queue = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties GPUProperties = {};
    VkPhysicalDeviceMemoryProperties MemoryProperties = {};

    WindowOld * MainWindowOld = nullptr;

    uint32_t GraphicsFamilyIndex = 0;

    std::vector<const char*> InstanceLayers;
    std::vector<const char*> InstaceExtensions;
    std::vector<const char*> DeviceExtensions;

    VkDebugReportCallbackEXT DebugReport = VK_NULL_HANDLE;
    VkDebugReportCallbackCreateInfoEXT DebugCallbackCreateInfo = {};
};

VULKAN_NS_END