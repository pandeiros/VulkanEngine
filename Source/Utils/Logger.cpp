/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/Logger.h"

#include <iostream>
#include <iomanip>
#include <string>

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
    std::cout << message << "\n";
}
