/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

/**
 * @file BuildOptions.h
 */

#ifdef VULKAN_ENABLE_DEBUG
#define VULKAN_ASSERT_ON_ERRORS
#define VULKAN_ENABLE_RUNTIME_DEBUG
#define VULKAN_ENABLE_PERFORMANCE_TESTING
#endif

#ifdef VULKAN_ENABLE_RUNTIME_DEBUG
#endif

// #SUPPORT Currently implementing VR mode using single viewport, because
// Android does not support multi viewport feature.
#ifdef VULKAN_VR_MODE_VIEWPORTS
#undef VULKAN_VR_MODE_VIEWPORTS
#endif

#ifndef __ANDROID__
#undef VULKAN_VR_MODE
#endif