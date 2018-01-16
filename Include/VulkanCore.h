/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#ifdef VULKAN_DEV
#define VULKAN_NS_BEGIN
#define VULKAN_NS_END
#define VULKAN_NS_USING
#define VULKAN_NS_NAME
#define VULKAN_NS_DECLARE(NAME)
#define VULKAN_NS_PREFIX(NAME) NAME
#else
#define VULKAN_NS_NAME vulkan
#define VULKAN_NS_BEGIN namespace VULKAN_NS_NAME {
#define VULKAN_NS_END   }
#define VULKAN_NS_USING using namespace VULKAN_NS_NAME
#define VULKAN_NS_DECLARE(NAME) using VULKAN_NS_NAME::NAME
#define VULKAN_NS_PREFIX(NAME) VULKAN_NS_NAME::NAME
#endif

#include "Platform.h"
#include "BuildOptions.h"
#include "Utils/LogTypes.h"

#include <iostream>
#include <vector>
#include <memory>

/**
 * @file VulkanCore.h
 */

/* Number of samples needs to be the same at image creation,
 * render pass creation and pipeline creation. */
#define VULKAN_SAMPLE_COUNT VK_SAMPLE_COUNT_1_BIT

VULKAN_NS_BEGIN

VK_DECLARE_LOG_CATEGORY(LogEngine);
VK_DECLARE_LOG_CATEGORY(LogVulkan);

// #TODO Move this function somewhere else.
template<typename T>
bool IsOfEnum(T Value, std::vector<T> Enums)
{
    for (auto & Enum : Enums)
    {
        if (Value == Enum)
            return true;
    }

    return false;
}

class Device;

/**
 * @class VulkanClass
 */
class VulkanClass
{
public:
    /**
     * Default constructors.
     */
    VulkanClass() = default;

    VulkanClass(std::shared_ptr<Device> device);

    /**
     * Default destructor.
     */
    ~VulkanClass() = default;

    virtual void Init() {};
    virtual void Destroy() {};
    void Update(float deltaTime);

    bool IsDirty() const;
    void SetDirty(bool isDirty);

    void SetUpdateEnabled(bool isUpdateEnabled);

    bool IsPendingKill() const;
    void SetPendingKill(bool pendingKill);

protected:
    virtual void Tick(float deltaTime) {};

    VkDevice device;

private:
    bool bIsDirty = false;
    bool bIsUpdateEnabled = false;
    bool bPendingKill = false;
};

VULKAN_NS_END