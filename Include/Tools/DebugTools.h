/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"

/**
 * @file DebugTools.h
 */

VULKAN_NS_BEGIN

/**
 * @class DebugTools
 */
class DebugTools
{
public:
    /**
     * Default constructor.
     */
    DebugTools() = default;

    /**
     * Default destructor.
     */
    ~DebugTools() = default;

    static VkResult Verify(VkResult result);
    static void Assert(const bool value, const char* message);
};

VULKAN_NS_END