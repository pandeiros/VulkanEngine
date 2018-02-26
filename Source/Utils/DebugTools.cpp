/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/DebugTools.h"
#include "Utils/Logger.h"

#include <cassert>
#include <algorithm>

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogDebug);

PerformanceSection::PerformanceSection(std::string name)
    : name(name), bActive(true), bPersistent(false)
{
    DebugTools::BeginPerformanceSection(*this);
}

PerformanceSection::PerformanceSection(std::string name, bool bPersistent)
    : name(name), bActive(true), bPersistent(bPersistent)
{
}

PerformanceSection::~PerformanceSection()
{
    if (!bPersistent)
    {
        DebugTools::EndPerformanceSection(*this);
    }
}

void PerformanceSection::BeginSection()
{
    DebugTools::BeginPerformanceSection(*this);
}

void PerformanceSection::EndSection()
{
    DebugTools::EndPerformanceSection(*this);
}

void PerformanceSection::Add(PerformanceSection& performanceSection)
{
    bool bFound = false;
    for (PerformanceSection& section : performanceSections)
    {
        if (section.bActive)
        {
            section.Add(performanceSection);
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        performanceSections.push_back(performanceSection);
    }
}

void PerformanceSection::End(PerformanceSection& performanceSection, std::chrono::steady_clock* timer)
{
    if (performanceSection == *this)
    {
        EndInternal(timer);
    }
    else
    {
        for (PerformanceSection& section : performanceSections)
        {
            if (section.bActive)
            {
                section.End(performanceSection, timer);
                break;
            }
        }
    }
}


void PerformanceSection::EndInternal(std::chrono::steady_clock* timer)
{
    bActive = false;
    endTime = timer->now();

    for (PerformanceSection& section : performanceSections)
    {
        if (section.bActive)
        {
            section.EndInternal(timer);
            break;
        }
    }
}

void PerformanceSection::Log(uint32_t level, std::chrono::steady_clock* timer)
{
    VK_LOG(LogDebug, Debug, "|%.*s%8.2f ms [%s]", level * 2, "                       ",
        std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count() * 1000.f, name.c_str());

    for (PerformanceSection& section : performanceSections)
    {
        section.Log(level + 1, timer);
    }
}


void PerformanceData::Log()
{
    VK_LOG(LogDebug, Debug, "|%8.2f ms [%s]", std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count() * 1000.f, name.c_str());

    for (PerformanceSection& section : performanceSections)
    {
        section.Log(1, &timer);
    }
}

PerformanceData::PerformanceData(std::string name)
    : name(name), timer(std::chrono::steady_clock()), bActive(true)
{
    startTime = timer.now();
    DebugTools::BeginPerformanceData(*this);
}

PerformanceData::~PerformanceData()
{
    DebugTools::EndPerformanceData(*this);
}

void PerformanceData::AddSection(PerformanceSection& performanceSection)
{
    bool bAdded = false;
    for (PerformanceSection& section : performanceSections)
    {
        if (section.bActive)
        {
            section.Add(performanceSection);
            bAdded = true;
            break;
        }
    }

    if (!bAdded)
    {
        performanceSections.push_back(performanceSection);
    }
}

void PerformanceData::EndSection(PerformanceSection& performanceSection)
{
    for (PerformanceSection& section : performanceSections)
    {
        if (section.bActive)
        {
            section.End(performanceSection, &timer);
        }
    }
}


std::vector<PerformanceData> DebugTools::performanceData = std::vector<PerformanceData>();

VARARG_BODY(void, DebugTools::Assert, const char*,
    VARARG_EXTRA(const char* file) VARARG_EXTRA(int line)
    VARARG_EXTRA(bool condition))
{
    if (!condition)
    {
        constexpr size_t MAX_BUFFER_SIZE = 1024;

        char buffer[MAX_BUFFER_SIZE];
        va_list argptr;
        va_start(argptr, Format);
        vsnprintf(buffer, MAX_BUFFER_SIZE, Format, argptr);
        va_end(argptr);

#ifdef _WIN32
        printf("[%s] %s: %s\n", Logger::GetVerbosityString(LogVerbosity::Fatal), "Assert", buffer);
#elif __ANDROID__
        __android_log_print(Logger::GetPlatformVerbosity(LogVerbosity::Fatal), VULKAN_ANDROID_TAG, "%s: %s", "Assert", buffer);
#endif

        assert(condition);
        //fflush(stdout);

        // #TODO Change this to platform dependent exit.
        //std::exit(-1);
    }
}

VkResult DebugTools::Verify(VkResult result)
{
#ifdef VULKAN_ENABLE_RUNTIME_DEBUG
    if (result < 0)
    {
        switch (result)
        {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_OUT_OF_HOST_MEMORY");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_OUT_OF_DEVICE_MEMORY");
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_INITIALIZATION_FAILED");
            break;
        case VK_ERROR_DEVICE_LOST:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_DEVICE_LOST");
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_MEMORY_MAP_FAILED");
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_LAYER_NOT_PRESENT");
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_EXTENSION_NOT_PRESENT");
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_FEATURE_NOT_PRESENT");
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_INCOMPATIBLE_DRIVER");
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_TOO_MANY_OBJECTS");
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_FORMAT_NOT_SUPPORTED");
            break;
        case VK_ERROR_FRAGMENTED_POOL:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_FRAGMENTED_POOL");
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_SURFACE_LOST_KHR");
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
            break;
        case VK_SUBOPTIMAL_KHR:
            VK_LOG(LogDebug, Fatal, "VK_SUBOPTIMAL_KHR");
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_OUT_OF_DATE_KHR");
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_VALIDATION_FAILED_EXT");
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            VK_LOG(LogDebug, Fatal, "VK_ERROR_INVALID_SHADER_NV");
            break;
        default:
            break;
        }

        VK_ASSERT(0, "Vulkan runtime error.");
    }

#endif // VULKAN_ENABLE_RUNTIME_DEBUG

    return result;
}

VkDebugReportFlagsEXT DebugTools::GetVulkanDebugFlagsEnabled()
{
    return
        VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT |
        0;
}

void DebugTools::BeginPerformanceData(PerformanceData& data)
{
    performanceData.push_back(data);
}

void DebugTools::EndPerformanceData(PerformanceData& data)
{
    auto iterator = std::find(performanceData.begin(), performanceData.end(), data);

    if (iterator != performanceData.end())
    {
        iterator->endTime = iterator->timer.now();
        iterator->bActive = false;
    }
    else
    {
        // #TODO Error;
    }
}

void DebugTools::BeginPerformanceSection(PerformanceSection& section)
{
    for (size_t i = 0; i < performanceData.size(); ++i)
    {
        if (performanceData[i].bActive)
        {
            section.startTime = performanceData[i].timer.now();
            performanceData[i].AddSection(section);
            break;
        }
    }
}

void DebugTools::EndPerformanceSection(PerformanceSection& section)
{
    for (size_t i = 0; i < performanceData.size(); ++i)
    {
        if (performanceData[i].bActive)
        {
            performanceData[i].EndSection(section);
            break;
        }
    }
}

void DebugTools::ClearPerformanceData()
{
    performanceData.clear();
}

void DebugTools::LogPerformanceData()
{
    for (PerformanceData& data : performanceData)
    {
        data.Log();
    }
}

