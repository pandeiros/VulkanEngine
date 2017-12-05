/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

/**
 * @file Platform.h
 */

#ifdef __ANDROID__
#define VULKAN_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#undef _WIN32

#elif _WIN32
#define VK_USE_PLATFORM_WIN32_KHR 1
#define VULKAN_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#include <Windows.h>

#endif

#ifdef _WIN32
#include <vulkan/vulkan.h>
#elif __ANDROID__
#include "vulkan_wrapper.h"
#endif