/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

/**
 * @file Platform.h
 */

#ifdef _WIN32

#define VK_USE_PLATFORM_WIN32_KHR 1
#define VULKAN_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#include <Windows.h>

#else
#error Platform not yet supported.
#endif

#include <vulkan/vulkan.h>