/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "VulkanMisc.h"
#include "VarArgs.h"

#include <chrono>
#include <map>
#include <stack>

/**
 * @file DebugTools.h
 */

VULKAN_NS_BEGIN

/**
 * Helper assertion and verify macros.
 */

#define VK_ASSERT(Condition, Format, ...) \
    DebugTools::Assert(__FILE__, __LINE__, Condition, Format, ##__VA_ARGS__);

#define VK_VERIFY(RESULT) DebugTools::Verify(RESULT)

#ifdef VULKAN_ENABLE_PERFORMANCE_TESTING

// Should not be in the same scope as other data.
#define VK_PERFORMANCE_DATA(NAME) PerformanceData performanceDataObject(NAME)
#define VK_PERFORMANCE_SECTION(NAME) PerformanceSection performanceSectionObject(NAME)

#else
#define VK_PERFORMANCE_BEGIN(NAME)
#define VK_PERFORMANCE_SECTION(NAME)
#endif

struct PerformanceSection
{
    PerformanceSection(std::string name);

    ~PerformanceSection();

    void Add(PerformanceSection& performanceSection);
    void End(PerformanceSection& performanceSection, std::chrono::steady_clock* timer);
    void EndInternal(std::chrono::steady_clock* timer);

    void Log(uint32_t level, std::chrono::steady_clock* timer);

    std::string name;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;

    bool bActive;

    std::vector<PerformanceSection> performanceSections;

    bool operator==(PerformanceSection& other) const
    {
        return name == other.name;
    }
};

struct PerformanceData
{
    PerformanceData(std::string name);

    ~PerformanceData();

    void AddSection(PerformanceSection& performanceSection);
    void EndSection(PerformanceSection& performanceSection);

    void Log();

    std::string name;
    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    bool bActive;

    std::vector<PerformanceSection> performanceSections;

    bool operator==(const PerformanceData& other) const
    {
        return name == other.name;
    }
};

/**
 * @class DebugTools
 */
class DebugTools
{
    //////////////////////////////////////////////////////////////////////////
    // Vulkan debug and assertions
    //////////////////////////////////////////////////////////////////////////

public:
    VARARG_DECL(static void, static void, {}, Assert, VARARG_NONE, const char*,
        VARARG_EXTRA(const char* file) VARARG_EXTRA(int line) VARARG_EXTRA(bool condition),
        VARARG_EXTRA(file) VARARG_EXTRA(line) VARARG_EXTRA(condition));

    static VkResult Verify(VkResult result);

    static VkDebugReportFlagsEXT GetVulkanDebugFlagsEnabled();

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////

public:
    static void BeginPerformanceData(PerformanceData& data);
    static void EndPerformanceData(PerformanceData& data);

    static void BeginPerformanceSection(PerformanceSection& section);
    static void EndPerformanceSection(PerformanceSection& section);

    static void ClearPerformanceData();
    static void LogPerformanceData();

protected:
    static std::vector<PerformanceData> performanceData;
};

VULKAN_NS_END