/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

 /**
  * @file AndroidUtils.h
  */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <streambuf>

#include "VulkanCore.h"
#include "AndroidCore.h"

#ifdef __ANDROID__
#include <unistd.h>
#include <jni.h>
#include <android_native_app_glue.h>
#include "gvr.h"
#include "gvr_controller.h"

extern "C" {
    NATIVE_METHOD(void, nativeOnCreate)
        (JNIEnv *env, jobject obj, jobject asset_mgr, jlong gvrContextPtr);
}

VULKAN_NS_BEGIN

struct wrap_vector_as_istream : std::streambuf
{
    wrap_vector_as_istream(std::vector<char> & vec ) {
        this->setg(&vec[0], &vec[0], &vec[0]+vec.size() );
    }
};

// Forward declaration.
class Application;

class AndroidUtils
{
public:
    static void Init();
    static void Close();

    static void Start();
    static void Update();

    static void Pause();
    static void Resume();

    static void Clean();

    static void InitControllerApi();

    static std::vector<char> GetFileStream(std::string fileToFind);

    static std::unique_ptr<Application> vulkanApplication;
    static android_app* nativeApplication;

    // GVR
    static gvr::ControllerApi* controllerApi;
    static std::unique_ptr<gvr::GvrApi> gvrApi;
    static gvr_context* gvrContext;

private:
    static bool isPaused;
};

VULKAN_NS_END

#endif //__ANDROID__

VK_DECLARE_LOG_CATEGORY(LogAndroid);

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#ifdef __ANDROID__

bool Android_process_command();
ANativeWindow* AndroidGetApplicationWindow();
FILE* AndroidFopen(const char* fname, const char* mode);
void AndroidGetWindowSize(int32_t& width, int32_t& height);
bool AndroidLoadFile(const char* filePath, std::string *data);

// Main entry point
int vulkan_android_main(int argc, char *argv[]);

#ifndef VK_API_VERSION_1_0
// On Android, NDK would include slightly older version of headers that is missing the definition.
#define VK_API_VERSION_1_0 VK_API_VERSION
#endif

#endif