/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/DebugTools.h"
#include "Utils/Logger.h"

#include <cassert>

VULKAN_NS_USING;

VULKAN_DECLARE_LOG_CATEGORY_STATIC(LogDebug);

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
#if VULKAN_ENABLE_RUNTIME_DEBUG
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
        //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        //VK_DEBUG_REPORT_DEBUG_BIT_EXT |
        0;
}
