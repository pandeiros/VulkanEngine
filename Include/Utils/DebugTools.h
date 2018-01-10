/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "VulkanMisc.h"
#include "VarArgs.h"

/**
 * @file DebugTools.h
 */

VULKAN_NS_BEGIN

/**
 * Helper assertion and verify macros.
 */

#define VK_ASSERT(Condition, Format, ...) \
    DebugTools::Assert(__FILE__, __LINE__, Condition, Format, ##__VA_ARGS__);

#define VK_VERIFY(RESULT)   DebugTools::Verify(RESULT)

/**
 * @class DebugTools
 */
class DebugTools
{
public:
    VARARG_DECL(static void, static void, {}, Assert, VARARG_NONE, const char*,
        VARARG_EXTRA(const char* file) VARARG_EXTRA(int line) VARARG_EXTRA(bool condition),
        VARARG_EXTRA(file) VARARG_EXTRA(line) VARARG_EXTRA(condition));

    static VkResult Verify(VkResult result);

    static VkDebugReportFlagsEXT GetVulkanDebugFlagsEnabled();
};

VULKAN_NS_END