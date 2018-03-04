/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

/**
 * @file Platform.h
 */

#ifdef __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR 1
#define VULKAN_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#undef _WIN32

#elif _WIN32
#pragma comment(linker, "/subsystem:console")

#define VK_USE_PLATFORM_WIN32_KHR 1
#define VULKAN_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define VULKAN_APP_NAME_LENGTH 80

#include <Windows.h>

#endif

// Vulkan header
#ifdef _WIN32
#include <vulkan/vulkan.h>
#elif __ANDROID__
#include "vulkan_wrapper.h"
#endif