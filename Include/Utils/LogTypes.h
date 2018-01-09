/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file LogTypes.h
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

struct LogCategory
{
public:
    LogCategory()
        : Category("LogDefault")
    {}

    LogCategory(const char* Category)
        : Category(Category)
    {}

    const char* GetCategoryName() const
    {
        return Category;
    }

protected:
    const char* Category;
};

#define VULKAN_DECLARE_LOG_CATEGORY(CategoryName)                   \
    extern struct LogCategory##CategoryName : public LogCategory    \
    {                                                               \
    public:                                                         \
        LogCategory##CategoryName() : LogCategory(#CategoryName) {}     \
    } CategoryName

#define VULKAN_DECLARE_LOG_CATEGORY_STATIC(CategoryName)                   \
    static struct LogCategory##CategoryName : public LogCategory    \
    {                                                               \
    public:                                                         \
        LogCategory##CategoryName() : LogCategory(#CategoryName) {}     \
    } CategoryName

#define VULKAN_DEFINE_LOG_CATEGORY(CategoryName) \
    LogCategory##CategoryName CategoryName;

#define VK_LOG(CategoryName, Verbosity, Format, ...) \
    Logger::Logf(__FILE__, __LINE__, CategoryName.GetCategoryName(), LogVerbosity::Verbosity, Format, ##__VA_ARGS__);

#define VULKAN_LOGGER(...) \
    Logger::Logf(__FILE__, __LINE__, ##__VA_ARGS__);

#ifdef __ANDROID__
#define VULKAN_PLATFORM_VERBOSITY android_LogPriority
#else
#define VULKAN_PLATFORM_VERBOSITY LogVerbosity
#endif

VULKAN_NS_END