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

enum LogVerbosity
{
    Verbose,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

#define VULKAN_DECLARE_LOG_CATEGORY_INTERNAL(CategoryName, Qualifier)                   \
    Qualifier struct LogCategory##CategoryName : public VULKAN_NS_PREFIX(LogCategory)   \
    {                                                                                   \
    public:                                                                             \
        LogCategory##CategoryName() : LogCategory(#CategoryName) {}                     \
    } CategoryName

#define VK_DECLARE_LOG_CATEGORY(CategoryName)                                           \
    VULKAN_DECLARE_LOG_CATEGORY_INTERNAL(CategoryName, static)

#define VK_LOG(CategoryName, Verbosity, Format, ...) \
    VULKAN_NS_PREFIX(Logger)::Logf(__FILE__, __LINE__, CategoryName.GetCategoryName(), VULKAN_NS_PREFIX(LogVerbosity)::Verbosity, Format, ##__VA_ARGS__);

#define VULKAN_LOGGER(...) \
    VULKAN_NS_PREFIX(Logger)::Logf(__FILE__, __LINE__, ##__VA_ARGS__);

#ifdef __ANDROID__
#define VULKAN_PLATFORM_VERBOSITY android_LogPriority
#else
#define VULKAN_PLATFORM_VERBOSITY LogVerbosity
#endif

VULKAN_NS_END