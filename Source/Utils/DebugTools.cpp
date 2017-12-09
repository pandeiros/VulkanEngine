/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/DebugTools.h"
#include <cassert>

VULKAN_NS_USING;

VkResult DebugTools::Verify(VkResult result)
{
#if VULKAN_ENABLE_RUNTIME_DEBUG
    if (result < 0)
    {
        switch (result)
        {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            Logger::Log("VK_ERROR_OUT_OF_HOST_MEMORY");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            Logger::Log("VK_ERROR_OUT_OF_DEVICE_MEMORY");
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            Logger::Log("VK_ERROR_INITIALIZATION_FAILED");
            break;
        case VK_ERROR_DEVICE_LOST:
            Logger::Log("VK_ERROR_DEVICE_LOST");
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            Logger::Log("VK_ERROR_MEMORY_MAP_FAILED");
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            Logger::Log("VK_ERROR_LAYER_NOT_PRESENT");
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            Logger::Log("VK_ERROR_EXTENSION_NOT_PRESENT");
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            Logger::Log("VK_ERROR_FEATURE_NOT_PRESENT");
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            Logger::Log("VK_ERROR_INCOMPATIBLE_DRIVER");
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            Logger::Log("VK_ERROR_TOO_MANY_OBJECTS");
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            Logger::Log("VK_ERROR_FORMAT_NOT_SUPPORTED");
            break;
        case VK_ERROR_FRAGMENTED_POOL:
            Logger::Log("VK_ERROR_FRAGMENTED_POOL");
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            Logger::Log("VK_ERROR_SURFACE_LOST_KHR");
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            Logger::Log("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
            break;
        case VK_SUBOPTIMAL_KHR:
            Logger::Log("VK_SUBOPTIMAL_KHR");
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            Logger::Log("VK_ERROR_OUT_OF_DATE_KHR");
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            Logger::Log("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            Logger::Log("VK_ERROR_VALIDATION_FAILED_EXT");
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            Logger::Log("VK_ERROR_INVALID_SHADER_NV");
            break;
        default:
            break;
        }

        VK_ASSERT(0, "Vulkan runtime error.");
    }

#endif // VULKAN_ENABLE_RUNTIME_DEBUG

    return result;
}

void DebugTools::Assert(const bool condition, const char* message)
{
    if (!condition)
    {
        Logger::Log(message);
        assert(condition);
        //fflush(stdout);

        // #TODO Change this to platform dependent exit.
        //std::exit(-1);
    }
}
