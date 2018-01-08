/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "AndroidCore.h"
#include "VarArgs.h"

#include <string>

/**
 * @file Logger.h
 */

VULKAN_NS_BEGIN

enum LogVerbosity
{
    Verbose,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

#define VULKAN_LOG(CategoryName, Verbosity, Format, ...) \
    Logger::Logf(__FILE__, __LINE__, CategoryName, LogVerbosity::Verbosity, Format, ##__VA_ARGS__);

#ifdef __ANDROID__
#define VULKAN_PLATFORM_VERBOSITY android_LogPriority
#else
#define VULKAN_PLATFORM_VERBOSITY LogVerbosity
#endif

/**
 * @class Logger
 */
class Logger
{
public:
    VARARG_DECL(static void, static void, {}, Logf, VARARG_NONE, const char*,
        VARARG_EXTRA(const char* File) VARARG_EXTRA(int Line) VARARG_EXTRA(const char* Category) VARARG_EXTRA(LogVerbosity Verbosity),
        VARARG_EXTRA(File) VARARG_EXTRA(Line) VARARG_EXTRA(Category) VARARG_EXTRA(Verbosity));

    //static void Log(const std::string message);
    static void Log(std::string& message);
    static void Log(const char* message);
    static void Log(uint32_t message);
    //static void Log(char message[]);

private:
    static const char* GetVerbosityString(LogVerbosity Verbosity);

    static VULKAN_PLATFORM_VERBOSITY GetPlatformVerbosity(LogVerbosity Verbosity);

    static void LogInternal(const std::string& message);
    //static void LogInternal(std::string message);

    static uint32_t indentation;
};

VULKAN_NS_END