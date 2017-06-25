/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanConfig.h"

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