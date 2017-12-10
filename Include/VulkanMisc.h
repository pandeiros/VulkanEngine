/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file VulkanMisc.h
 */

VULKAN_NS_BEGIN

#define GET_3RD_ARG(A, B, C, ...)                   C
#define GET_4TH_ARG(A, B, C, D, ...)                D

// #TODO Change log to assert.
#define VULKAN_GET_INSTANCE_PROC_ADDR(INSTANCE, ENTRY_POINT)                                                   \
    {                                                                                                   \
        fvk##ENTRY_POINT = (PFN_vk##ENTRY_POINT)vkGetInstanceProcAddr(INSTANCE, "vk" #ENTRY_POINT);     \
        if (fvk##ENTRY_POINT == NULL)                                                                   \
        {                                                                                               \
            Logger::Log("vkGetDeviceProcAddr failed to find vk" #ENTRY_POINT);                          \
            /* Exit? */                                                                                 \
        }                                                                                               \
    }

VULKAN_NS_END