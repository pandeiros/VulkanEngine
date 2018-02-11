/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#define VULKAN_ENGINE_VERSION VK_MAKE_VERSION(0, 0, 2)
#define VULKAN_ENGINE_NAME "Vulkan Engine"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "World.h"
#include "Rendering/Renderer.h"
#include "InputManager.h"

#include <vector>
#include <queue>

/**
 * @file Engine.h
 */

VULKAN_NS_BEGIN

/**
 * @struct LayerProperties
 */
struct LayerProperties
{
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

/**
 * @class Engine
 */
class Engine
{
public:
    Engine(uint32_t frameRate, bool useFixedFrameRate = false);
    ~Engine();

    void Init(Instance* instance);

private:
    DevicePtr device;

    //////////////////////////////////////////////////////////////////////////
    // Update and frame rate
    //////////////////////////////////////////////////////////////////////////

public:
    void Update();

    void UseFixedFrameRate(bool useFixedFrameRate);
    void SetFrameRate(uint32_t newFrameRate);
    void SetMaxFrameRate(uint32_t maxFrameRate);

    float GetFPS() const;
    float GetTimeFromStart() const;

private:
    void UpdateInternal(float deltaTime);
    float GetMinDeltaTime() const;

    bool bEnabled = false;

    bool bUseFixedFrameRate = false;
    uint32_t frameRate = 60;
    uint32_t maxFPS = 0;
    float timePerFrame = 0.f;
    float minTimePerFrame = 0.f;
    float timeSinceLastUpdate = 0.f;

    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point lastFrameTime;
    std::chrono::steady_clock::time_point startTime;

    std::queue<float> frameTimes;
    float fpsTime = 0;
    float currentFPS;

    //////////////////////////////////////////////////////////////////////////
    // Main objects
    //////////////////////////////////////////////////////////////////////////

public:
    void RegisterObject(VulkanClass* object);

private:
    std::vector<VulkanClass*> applicationObjects;

    //////////////////////////////////////////////////////////////////////////
    // World
    //////////////////////////////////////////////////////////////////////////

public:
    World* GetWorld();

private:
    std::unique_ptr<World> world;

    //////////////////////////////////////////////////////////////////////////
    // Rendering
    //////////////////////////////////////////////////////////////////////////

public:
    Renderer* GetRenderer();
    Window* GetWindow();

private:
    std::unique_ptr<Renderer> renderer;
    Window* window;

    //////////////////////////////////////////////////////////////////////////
    // Input
    //////////////////////////////////////////////////////////////////////////

public:
    InputManager* GetInputManager();

private:
    std::unique_ptr<InputManager> inputManager;

    //////////////////////////////////////////////////////////////////////////
    // System properties
    //////////////////////////////////////////////////////////////////////////

public:
    void ValidateInstanceProperties(std::vector<const char*>& instanceLayers, std::vector<const char*>& instaceExtensions);
    void InitPhysicalDevices(VkInstance instance);

    PhysicalDevice* GetPhysicalDevice(uint32_t deviceIndex);

    void LogInstanceProperties();
    void LogDeviceProperties();

private:
    void InitInstanceProperties();

    VkResult EnumerateInstanceLayers();
    VkResult EnumerateInstanceExtensions(LayerProperties& layerProperties);

    std::vector<LayerProperties> instanceProperties;
    std::vector<VkExtensionProperties> globalInstanceExtensions;

    std::vector<std::unique_ptr<PhysicalDevice>> physicalDevices;
};

VULKAN_NS_END