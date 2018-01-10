/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "LogTypes.h"
#include "AndroidCore.h"
#include "VarArgs.h"

#include <string>

/**
 * @file Logger.h
 */

VULKAN_NS_BEGIN

/**
 * @class Logger
 */
class Logger
{
public:
    VARARG_DECL(static void, static void, {}, Logf, VARARG_NONE, const char*,
        VARARG_EXTRA(const char* file) VARARG_EXTRA(int line) VARARG_EXTRA(const char* category) VARARG_EXTRA(LogVerbosity verbosity),
        VARARG_EXTRA(file) VARARG_EXTRA(line) VARARG_EXTRA(category) VARARG_EXTRA(verbosity));

    VARARG_DECL(static void, static void, {}, Logf, VARARG_NONE, const char*,
        VARARG_EXTRA(const char* file) VARARG_EXTRA(int line) VARARG_EXTRA(LogVerbosity verbosity),
        VARARG_EXTRA(file) VARARG_EXTRA(line) VARARG_EXTRA("") VARARG_EXTRA(verbosity));

    VARARG_DECL(static void, static void, {}, Logf, VARARG_NONE, const char*,
        VARARG_EXTRA(const char* file) VARARG_EXTRA(int line),
        VARARG_EXTRA(file) VARARG_EXTRA(line) VARARG_EXTRA("") VARARG_EXTRA(LogVerbosity::Info));

    static const char* GetVerbosityString(LogVerbosity verbosity);
    static LogVerbosity GetVerbosity(VkDebugReportFlagsEXT Flags);
    static VULKAN_PLATFORM_VERBOSITY GetPlatformVerbosity(LogVerbosity verbosity);

    static void LogTest();

private:
    static int GetVerbosityColor(LogVerbosity verbosity);
};

VULKAN_NS_END