/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "VulkanMisc.h"

/**
 * @file DebugTools.h
 */

VULKAN_NS_BEGIN

/**
 * Helper assertion and verify macros.
 */
#define VK_ASSERT_INTERNAL_1(COND)          DebugTools::Assert(COND)
#define VK_ASSERT_INTERNAL_2(COND, MSG)     DebugTools::Assert(COND, MSG)

#define VK_ASSERT_INTERNAL(...)             GET_3RD_ARG(__VA_ARGS__,    \
                                                VK_ASSERT_INTERNAL_2,   \
                                                VK_ASSERT_INTERNAL_1, )

#define VK_ASSERT(...)                      VK_ASSERT_INTERNAL(__VA_ARGS__)(__VA_ARGS__)
#define VK_VERIFY(RESULT)                   DebugTools::Verify(RESULT)

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
    static void Assert(const bool condition, const char* message = "");
};

VULKAN_NS_END