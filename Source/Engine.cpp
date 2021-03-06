/**
* Vulkan Engine
*
* Copyright (C) 2016-2018 Pawel Kaczynski
*/

#include "Engine.h"

#include "Core.h"

#include <cstdlib>
#include <algorithm>

VULKAN_NS_USING;

Engine::Engine(uint32_t frameRate, bool useFixedFrameRate /*= false*/)
{
    InitInstanceProperties();

    SetFrameRate(frameRate);
    UseFixedFrameRate(useFixedFrameRate);

    timer = std::chrono::steady_clock();
    lastFrameTime = startTime = timer.now();

    bEnabled = true;
}

Engine::~Engine()
{
    bEnabled = false;

    World *pWorld = world.release();
    if (pWorld)
    {
        delete pWorld;
    }

    Renderer* pRenderer = renderer.release();
    if (pRenderer)
    {
        delete pRenderer;
    }
}

void Engine::Init(Instance* instance)
{
    this->device = device;

    renderer.reset(new Renderer(instance->GetDevicePtr(), this));
    world.reset(new World(instance->GetDevicePtr(), this));
    inputManager.reset(new InputManager);
    window = instance->GetWindow();

    using namespace std::placeholders;
    inputManager->BindEvent<float>(InputCode::GVR_BUTTON_APP, InputEventDelegate<float>(InputEvent::ON_PRESSED, std::bind(&Engine::OnAppButtonPressed, this, _1, _2, _3)));
}

void Engine::Update()
{
    if (bPerformanceDataLogRequested)
    {
        VK_LOG(LogEngine, Debug, "FPS: %.0f", GetFPS());
        DebugTools::LogPerformanceData();
        bPerformanceDataLogRequested = false;
    }

    DebugTools::ClearPerformanceData();

    VK_PERFORMANCE_DATA("Engine");

    std::chrono::duration<float> frameTimeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(timer.now() - lastFrameTime);
    lastFrameTime = timer.now();

    timeSinceLastUpdate += frameTimeDiff.count();

    // Uncapped (almost frame rate).
    float deltaTime = frameTimeDiff.count() < GetMinDeltaTime() ? GetMinDeltaTime() : frameTimeDiff.count();

    // Fixed frame rate
    if (bUseFixedFrameRate)
    {
        deltaTime = timePerFrame;
    }
    // Max frame rate is set.
    else if (maxFPS > 0)
    {
        deltaTime = minTimePerFrame;
    }

    fpsTime += frameTimeDiff.count();

    // We only proceed if at least deltaTime seconds have passed after last frame.
    if (timeSinceLastUpdate >= deltaTime)
    {
        timeSinceLastUpdate -= deltaTime;
        frameTimes.push(deltaTime);

        UpdateInternal(deltaTime);

        if (fpsTime >= 1.f)
        {
            currentFPS = (float)frameTimes.size() / fpsTime;

            float oldestFrameTime = frameTimes.front();
            frameTimes.pop();
            fpsTime -= oldestFrameTime;
        }
    }
}

void Engine::UseFixedFrameRate(bool useFixedFrameRate)
{
    bUseFixedFrameRate = useFixedFrameRate;
}

void Engine::SetFrameRate(uint32_t newFrameRate)
{
    VK_ASSERT(newFrameRate > 0, "Frame rate has to be greater than 0!");

    frameRate = newFrameRate;
    timePerFrame = 1.f / frameRate;

    if (timePerFrame < GetMinDeltaTime())
    {
        VK_LOG(LogEngine, Warning, "Time per frame (%.2f) for specified maximum framerate is below minimum (%.2f).", timePerFrame, GetMinDeltaTime());
    }
}

void Engine::SetMaxFrameRate(uint32_t maxFrameRate)
{
    maxFPS = maxFrameRate;

    if (maxFPS > 0)
    {
        minTimePerFrame = 1.f / maxFPS;

        if (minTimePerFrame < GetMinDeltaTime())
        {
            VK_LOG(LogEngine, Warning, "Time per frame (%.2f) for specified maximum framerate is below minimum (%.2f).", minTimePerFrame, GetMinDeltaTime());
        }
    }
    else
    {
        minTimePerFrame = GetMinDeltaTime();
    }
}

float Engine::GetFPS() const
{
    return currentFPS;
}

float Engine::GetTimeFromStart() const
{
    return std::chrono::duration_cast<std::chrono::duration<float>>(timer.now() - startTime).count();
}

void Engine::UpdateInternal(float deltaTime)
{
    VK_PERFORMANCE_SECTION("Engine update");

    if (!bEnabled)
    {
        return;
    }

    if (deltaTime < GetMinDeltaTime())
    {
        deltaTime = GetMinDeltaTime();
    }

    for (auto& object : applicationObjects)
    {
        if (object)
        {
            object->Update(deltaTime);
        }
    }

    world->Update(deltaTime);

    renderer->Update(deltaTime);

    world->SetDirty(false);
}

float Engine::GetMinDeltaTime() const
{
    return 0.0001f;
}

void Engine::RegisterObject(VulkanClass* object)
{
    if (std::find(applicationObjects.begin(), applicationObjects.end(), object) == applicationObjects.end())
    {
        applicationObjects.push_back(object);
    }
}

World* Engine::GetWorld()
{
    return world.get();
}

Renderer* Engine::GetRenderer()
{
    return renderer.get();
}

Window* Engine::GetWindow()
{
    return window;
}

InputManager* Engine::GetInputManager()
{
    return inputManager.get();
}

void Engine::OnAppButtonPressed(InputCode inputCode, InputEvent event, float value)
{
    RequestPerformanceDataLog();
}

void Engine::RequestPerformanceDataLog()
{
    bPerformanceDataLogRequested = true;
}

void Engine::LogInstanceProperties()
{
    if (instanceProperties.size())
    {
        InitInstanceProperties();
    }

    VK_LOG(LogEngine, Debug, "Instance properties:");
    for (LayerProperties& instanceProperty : instanceProperties)
    {
        VK_LOG(LogEngine, Debug, "  %s", instanceProperty.properties.layerName);
        for (VkExtensionProperties& extensionProperty : instanceProperty.extensions)
        {
            VK_LOG(LogEngine, Debug, "    %s", extensionProperty.extensionName);
        }
    }

    VK_LOG(LogEngine, Debug, "Global extensions:");
    for (VkExtensionProperties extension : globalInstanceExtensions)
    {
        VK_LOG(LogEngine, Debug, "  %s", extension.extensionName);
    }
}

void Engine::LogDeviceProperties()
{
    VK_LOG(LogEngine, Debug, "Physical devices:");
    for (auto& physicalDevice : physicalDevices)
    {
        physicalDevice->LogInfo();
    }
}

void Engine::InitInstanceProperties()
{
    VK_VERIFY(EnumerateInstanceLayers());

    uint32_t globalExtensionCount = 0;
    VkResult result;
    do
    {
        VK_VERIFY(vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionCount, nullptr));

        if (globalExtensionCount == 0)
        {
            return;
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
            return std::find_if(instancePropertiesCopy.begin(), instancePropertiesCopy.end(), [&layer](const LayerProperties& layerProperties)
            {
                return layerProperties.properties.layerName == layer;
            }) != instancePropertiesCopy.end();
        }), instanceLayers.end());

        std::vector<const char*> instanceLayersDiff = Math::Diff<const char*>(instanceLayersCopy, instanceLayers);
        if (instanceLayersDiff.size() > 0)
        {
            VK_LOG(LogEngine, Verbose, "Invalid instance layers:");
            for (auto& layer : instanceLayersDiff)
            {
                VK_LOG(LogEngine, Verbose, " %s", layer);
            }
        }
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

        std::vector<const char*> instanceExtensionsDiff = Math::Diff<const char*>(instanceExtensionsCopy, instanceExtensions);
        if (instanceExtensionsDiff.size() > 0)
        {
            VK_LOG(LogEngine, Verbose, "Invalid instance extensions:");
            for (auto& extension : instanceExtensionsDiff)
            {
                VK_LOG(LogEngine, Verbose, " %s", extension);
            }
        }
    }
}

void Engine::InitPhysicalDevices(VkInstance instance)
{
    std::vector<VkPhysicalDevice> vkPhysicalDevices;

    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);
    vkPhysicalDevices.resize(physicalDevicesCount);
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, vkPhysicalDevices.data());

    for (VkPhysicalDevice vkPhysicalDevice : vkPhysicalDevices)
    {
        std::unique_ptr<PhysicalDevice> physicalDevice(new PhysicalDevice(vkPhysicalDevice));

        physicalDevices.push_back(std::move(physicalDevice));
    }

    VK_ASSERT(physicalDevices.size() > 0, "No viable physical device found on the system!");
}

PhysicalDevice* VULKAN_NS_NAME::Engine::GetPhysicalDevice(uint32_t deviceIndex)
{
    VK_ASSERT(deviceIndex < physicalDevices.size(), "Invalid physical device index.");

    return physicalDevices[deviceIndex].get();
}

VkResult Engine::EnumerateInstanceLayers()
{
    VkLayerProperties* properties = nullptr;
    VkResult result;
    uint32_t layerCount = 0;

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
