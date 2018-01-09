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

    char* categorySuffix = ": ";
    if (strlen(category) == 0)
    {
        categorySuffix = "";
    }

    if (verbosity == LogVerbosity::Fatal)
    {
        // #TODO Change this to some exit method.
        VK_ASSERT(0, buffer);
    }
    else
    {
#ifdef _WIN32
        printf("[%s] %s%s%s\n", GetVerbosityString(verbosity), category, categorySuffix, buffer);
#elif __ANDROID__
        __android_log_print(GetPlatformVerbosity(verbosity), VULKAN_ANDROID_TAG, "%s%s%s", category, categorySuffix, buffer);
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

const char* Logger::GetVerbosityString(LogVerbosity Verbosity)
{
    switch (Verbosity)
    {
    case LogVerbosity::Verbose:
        return "Verbose";
    case LogVerbosity::Debug:
        return "Debug";
    case LogVerbosity::Info:
        return "Info";
    case LogVerbosity::Warning:
        return "Warning";
    case LogVerbosity::Error:
        return "Error";
    case LogVerbosity::Fatal:
        return "FATAL";
    default:
        return "Info";
    }
}

VULKAN_PLATFORM_VERBOSITY Logger::GetPlatformVerbosity(LogVerbosity Verbosity)
{
#ifdef __ANDROID__
    switch (Verbosity)
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
    return Verbosity;
#endif
}
