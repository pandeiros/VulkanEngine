/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

 /**
  * @file AndroidCore.h
  */

#define VULKAN_ANDROID_TAG "VULKAN_ENGINE"

#ifdef __ANDROID__

#include <android/log.h>

 // Android specific definitions & helpers.
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, VULKAN_ANDROID_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, VULKAN_ANDROID_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, VULKAN_ANDROID_TAG, __VA_ARGS__))

 // Replace printf to logcat output.
//#define printf(...) __android_log_print(ANDROID_LOG_INFO, VULKAN_ANDROID_TAG, __VA_ARGS__);

#endif
