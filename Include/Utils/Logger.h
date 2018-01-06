/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include <string>

#include "AndroidCore.h"

/**
 * @file Logger.h
 */

VULKAN_NS_BEGIN

// #TODO Fix non-Android version.
#ifdef __ANDROID__
#define VULKAN_LOG(FORMAT, ...) LOGI(FORMAT, ##__VA_ARGS__);
#else
#define VULKAN_LOG(FORMAT, ...)
#endif

/**
 * @class Logger
 */
class Logger
{
public:
    //static void Log(const std::string message);
    static void Log(std::string& message);
    static void Log(const char* message);
    static void Log(uint32_t message);
    //static void Log(char message[]);

private:
    static void LogInternal(const std::string& message);
    //static void LogInternal(std::string message);

    static uint32_t indentation;
};

VULKAN_NS_END