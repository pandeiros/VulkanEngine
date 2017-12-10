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
#include "Utils/Logger.h"
#include "Utils/DebugTools.h"

#include <iostream>
#include <vector>

// #TODO MOVE TO MATH FILE OR STH
constexpr double PI = 3.14159265359;

/**
 * @file VulkanCore.h
 */

VULKAN_NS_BEGIN

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
    bool IsDirty() const;
    void SetDirty(bool bDirtyFlag);

private:
    bool bIsDirty = false;
};

VULKAN_NS_END