/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/Logger.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "AndroidCore.h"

VULKAN_NS_USING;

uint32_t Logger::indentation = 0;

void Logger::Log(const char* message)
{
    LogInternal(message);
}

void Logger::Log(uint32_t message)
{
    // TODO Fix for Android
    //LogInternal(std::to_string(message));
}

void Logger::Log(std::string& message)
{
    LogInternal(message);
}

void Logger::LogInternal(const std::string& message)
{
    //std::cout << std::setw(indentation * 2) << "" << std::setw(0) << message << "\n";

    // #TODO Implement global application TAG (i.e. VK-SAMPLE below).
#ifdef __ANDROID__
    LOGI("%s", message.c_str());
    //__android_log_print(ANDROID_LOG_INFO, "VK-SAMPLE", "%s", message.c_str());
#else
    std::cout << message << "\n";
#endif
}
