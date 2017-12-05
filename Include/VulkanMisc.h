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

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
    {                                                                          \
        info.fp##entrypoint =                                                  \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
        if (info.fp##entrypoint == NULL) {                                     \
            std::cout << "vkGetDeviceProcAddr failed to find vk" #entrypoint;  \
            exit(-1);                                                          \
        }                                                                      \
    }

VULKAN_NS_END