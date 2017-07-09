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
#include "Tools/Logger.h"
#include "Tools/DebugTools.h"

#include <iostream>
#include <vector>

// #TODO MOVE TO MATH FILE OR STH
constexpr double PI = 3.14159265359;

/**
 * @file VulkanConfig.h
 */

VULKAN_NS_BEGIN

void ErrorCheck(VkResult Result);

uint32_t GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * PhysicalDeviceMemoryProperties, const VkMemoryRequirements * MemoryRequirements, const VkMemoryPropertyFlags MemoryProperties);

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

VULKAN_NS_END