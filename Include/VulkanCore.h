/*
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#ifdef VULKAN_DEV
#define VULKAN_NS_BEGIN
#define VULKAN_NS_END
#define VULKAN_NS_USING
#else
#define VULKAN_NS_BEGIN namespace vulkan {
#define VULKAN_NS_END   }
#define VULKAN_NS_USING using namespace vulkan
#endif

#include "Platform.h"
#include "BuildOptions.h"
#include "Utils/LogTypes.h"

#include <iostream>
#include <vector>

/**
 * @file VulkanCore.h
 */

/* Number of samples needs to be the same at image creation,
 * renderpass creation and pipeline creation. */
#define VULKAN_SAMPLE_COUNT VK_SAMPLE_COUNT_1_BIT

VULKAN_NS_BEGIN

VULKAN_DECLARE_LOG_CATEGORY(LogEngine);
VULKAN_DECLARE_LOG_CATEGORY(LogVulkan);

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

/**
 * @class VulkanObject
 */
class VulkanObject
{
public:
    virtual void Init() {};
    virtual void Destroy() {};
    void Update();

    bool IsDirty() const;
    void SetDirty(bool isDirty);

    void SetUpdateEnabled(bool isUpdateEnabled);

    bool IsPendingKill() const;
    void SetPendingKill(bool pendingKill);

protected:
    virtual void Tick() {};

private:
    bool bIsDirty = false;
    bool bIsUpdateEnabled = false;
    bool bPendingKill = false;
};

VULKAN_NS_END