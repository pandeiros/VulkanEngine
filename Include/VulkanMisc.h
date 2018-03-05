/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file VulkanMisc.h
 */

VULKAN_NS_BEGIN

#define GET_3RD_ARG(A, B, C, ...)                   C
#define GET_4TH_ARG(A, B, C, D, ...)                D

#define TEXT_PASTE(x) #x
#define VULKAN_TEXT(x) TEXT_PASTE(x)

#define VULKAN_GET_INSTANCE_PROC_ADDR(INSTANCE, ENTRY_POINT)                                            \
    {                                                                                                   \
        vk##ENTRY_POINT = (PFN_vk##ENTRY_POINT)vkGetInstanceProcAddr(INSTANCE, "vk" #ENTRY_POINT);      \
        if (! vk##ENTRY_POINT)                                                                          \
        {                                                                                               \
            VK_LOG(LogEngine, Fatal, "vkGetInstanceProcAddr failed to find vk" #ENTRY_POINT);           \
        }                                                                                               \
    }

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