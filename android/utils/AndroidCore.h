/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

 /**
  * @file AndroidCore.h
  */

#ifdef __ANDROID__  // #TODO Change this logging macros.

#include <android/log.h>

 // Android specific definitions & helpers.
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VK-SAMPLE", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VK-SAMPLE", __VA_ARGS__))

 // Replace printf to logcat output.
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "VK-SAMPLE", __VA_ARGS__);

#endif
