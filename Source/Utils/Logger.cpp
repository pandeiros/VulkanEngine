/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/Logger.h"
#include "Utils/DebugTools.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include <cstdarg>

VULKAN_NS_USING;

VARARG_BODY(void, Logger::Logf, const char*,
    VARARG_EXTRA(const char* file) VARARG_EXTRA(int line)
    VARARG_EXTRA(const char* category) VARARG_EXTRA(LogVerbosity verbosity))
{
#ifndef VULKAN_NO_LOGGING
    constexpr size_t MAX_BUFFER_SIZE = 1024;

    char buffer[MAX_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, Format);
    vsnprintf(buffer, MAX_BUFFER_SIZE, Format, argptr);
    va_end(argptr);

    if (verbosity == LogVerbosity::Fatal)
    {
        VK_ASSERT(0, buffer);
    }
    else
    {
#ifdef _WIN32
        HANDLE winConsole;
        winConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(winConsole, GetVerbosityColor(verbosity));
        printf("[%7.7s] %s%s%s\n", GetVerbosityString(verbosity), category, (strlen(category) == 0 ? "" : ": "), buffer);
        SetConsoleTextAttribute(winConsole, 15);

        if (verbosity == LogVerbosity::Error)
        {
            MessageBox(NULL, buffer, "Error", 0);

#ifdef VULKAN_ASSERT_ON_ERRORS
            VK_ASSERT(0, buffer);
#endif

        }
#elif __ANDROID__
        __android_log_print(GetPlatformVerbosity(verbosity), VULKAN_ANDROID_TAG, "%s%s%s", category, (strlen(category) == 0 ? "" : ": "), buffer);
#endif
    }

    //if (Verbosity != ELogVerbosity::Fatal)
    //{
    //    // SetColour is routed to GWarn just like the other verbosities and handled in the
    //    // device that does the actual printing.
    //    FOutputDevice* LogDevice = NULL;
    //    switch (Verbosity)
    //    {
    //    case ELogVerbosity::Error:
    //    case ELogVerbosity::Warning:
    //    case ELogVerbosity::Display:
    //    case ELogVerbosity::SetColor:
    //        if (GWarn)
    //        {
    //            LogDevice = GWarn;
    //            break;
    //        }
    //    default:
    //    {
    //        LogDevice = GLog;
    //    }
    //    break;
    //    }
    //    GROWABLE_LOGF(LogDevice->Log(Category, Verbosity, Buffer))
    //}
    //else
    //{
    //    // Keep Message buffer small, in some cases, this code is executed with 16KB stack.
    //    TCHAR Message[4096];
    //    {
    //        // Simulate Sprintf_s
    //        // @todo: implement platform independent sprintf_S
    //        // We're using one big shared static buffer here, so guard against re-entry
    //        FScopeLock MsgLock(&MsgLogfStaticBufferGuard);
    //        // Print to a large static buffer so we can keep the stack allocation below 16K
    //        GET_VARARGS(MsgLogfStaticBuffer, ARRAY_COUNT(MsgLogfStaticBuffer), ARRAY_COUNT(MsgLogfStaticBuffer) - 1, Fmt, Fmt);
    //        // Copy the message to the stack-allocated buffer)
    //        FCString::Strncpy(Message, MsgLogfStaticBuffer, ARRAY_COUNT(Message) - 1);
    //        Message[ARRAY_COUNT(Message) - 1] = '\0';
    //    }

    //    StaticFailDebug(TEXT("Fatal error:"), File, Line, Message, false);
    //}
#endif
}

const char* Logger::GetVerbosityString(LogVerbosity verbosity)
{
    switch (verbosity)
    {
    case LogVerbosity::Verbose:
        return "Verbose";
    case LogVerbosity::Debug:
        return " Debug ";
    case LogVerbosity::Info:
        return " Info  ";
    case LogVerbosity::Warning:
        return "Warning";
    case LogVerbosity::Error:
        return " Error ";
    case LogVerbosity::Fatal:
        return " FATAL ";
    default:
        return " Info  ";
    }
}

LogVerbosity Logger::GetVerbosity(VkDebugReportFlagsEXT Flags)
{
    if (Flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        return LogVerbosity::Info;
    }
    else if (Flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        return LogVerbosity::Warning;
    }
    else if (Flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        return LogVerbosity::Warning;
    }
    else if (Flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        return LogVerbosity::Error;
    }
    else if (Flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        return LogVerbosity::Debug;
    }

    return LogVerbosity::Info;
}

void Logger::LogTest()
{
    VULKAN_LOGGER("Logger", LogVerbosity::Verbose, "Test");
    VULKAN_LOGGER("Logger", LogVerbosity::Debug, "Test");
    VULKAN_LOGGER("Logger", LogVerbosity::Info, "Test");
    VULKAN_LOGGER("Logger", LogVerbosity::Warning, "Test");
    VULKAN_LOGGER("Logger", LogVerbosity::Error, "Test");
}

VULKAN_PLATFORM_VERBOSITY Logger::GetPlatformVerbosity(LogVerbosity verbosity)
{
#ifdef __ANDROID__
    switch (verbosity)
    {
    case LogVerbosity::Verbose:
        return android_LogPriority::ANDROID_LOG_VERBOSE;
    case LogVerbosity::Debug:
        return android_LogPriority::ANDROID_LOG_DEBUG;
    case LogVerbosity::Info:
        return android_LogPriority::ANDROID_LOG_INFO;
    case LogVerbosity::Warning:
        return android_LogPriority::ANDROID_LOG_WARN;
    case LogVerbosity::Error:
        return android_LogPriority::ANDROID_LOG_ERROR;
    case LogVerbosity::Fatal:
        return android_LogPriority::ANDROID_LOG_FATAL;
    default:
        return android_LogPriority::ANDROID_LOG_DEFAULT;
    }
#else
    return verbosity;
#endif
}

int Logger::GetVerbosityColor(LogVerbosity verbosity)
{
    int colorCode = -1;

#ifdef _WIN32
    switch (verbosity)
    {
    case LogVerbosity::Verbose:
        colorCode = 8;
        break;
    case LogVerbosity::Debug:
        colorCode = 11;
        break;
    case LogVerbosity::Info:
        colorCode = 10;
        break;
    case LogVerbosity::Warning:
        colorCode = 14;
        break;
    case LogVerbosity::Error:
        colorCode = 12;
        break;
    case LogVerbosity::Fatal:
        colorCode = 12;
        break;
    default:
        colorCode = 8;
        break;
    }
#endif

    return colorCode;
}
