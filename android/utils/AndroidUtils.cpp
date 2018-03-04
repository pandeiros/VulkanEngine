/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "AndroidUtils.h"

#include "Engine.h"
#include "Core.h"

#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <iostream>

#ifdef __ANDROID__
#include <unordered_map>
#include "shaderc/shaderc.hpp"
#include "Application.h"
#include <sys/time.h>
#endif

VULKAN_NS_USING;

using namespace std;

#ifdef __ANDROID__
// Helper class to forward the cout/cerr output to logcat derived from:
// http://stackoverflow.com/questions/8870174/is-stdcout-usable-in-android-ndk
class AndroidBuffer : public std::streambuf {
public:
    AndroidBuffer(android_LogPriority priority) {
        priority_ = priority;
        this->setp(buffer_, buffer_ + kBufferSize - 1);
    }

private:
    static const int32_t kBufferSize = 128;
    int32_t overflow(int32_t c) {
        if (c == traits_type::eof()) {
            *this->pptr() = traits_type::to_char_type(c);
            this->sbumpc();
        }
        return this->sync() ? traits_type::eof() : traits_type::not_eof(c);
    }

    int32_t sync() {
        int32_t rc = 0;
        if (this->pbase() != this->pptr()) {
            char writebuf[kBufferSize + 1];
            memcpy(writebuf, this->pbase(), this->pptr() - this->pbase());
            writebuf[this->pptr() - this->pbase()] = '\0';

            rc = __android_log_write(priority_, "std", writebuf) > 0;
            this->setp(buffer_, buffer_ + kBufferSize - 1);
        }
        return rc;
    }

    android_LogPriority priority_ = ANDROID_LOG_INFO;
    char buffer_[kBufferSize];
};

void Android_handle_cmd(android_app *app, int32_t cmd) {
    switch (cmd)
    {
        case APP_CMD_INPUT_CHANGED:
            LOGW("APP_CMD_INPUT_CHANGED");
            break;

        case APP_CMD_INIT_WINDOW:
            LOGW("APP_CMD_INIT_WINDOW");
            // The window is being shown, get it ready.
            AndroidUtils::Init();
            break;

        case APP_CMD_TERM_WINDOW:
            LOGW("APP_CMD_TERM_WINDOW");
            // The window is being hidden or closed, clean it up.
            AndroidUtils::Close();
            break;

        case APP_CMD_GAINED_FOCUS:
            LOGW("APP_CMD_GAINED_FOCUS");
            // The window is being hidden or closed, clean it up.
            AndroidUtils::Resume();
            break;

        case APP_CMD_LOST_FOCUS:
            LOGW("APP_CMD_LOST_FOCUS");
            // The window is being hidden or closed, clean it up.
            AndroidUtils::Pause();
            break;

        case APP_CMD_START:
            LOGW("APP_CMD_START");
            AndroidUtils::Start();
            break;

        case APP_CMD_RESUME:
            LOGW("APP_CMD_RESUME");
            AndroidUtils::Resume();
            break;

        case APP_CMD_SAVE_STATE:
            // #TODO Handle this.
            LOGW("APP_CMD_SAVE_STATE");
            break;

        case APP_CMD_PAUSE:
            LOGW("APP_CMD_PAUSE");
            AndroidUtils::Pause();
            break;

        case APP_CMD_STOP:
            LOGW("APP_CMD_STOP");
            break;

        case APP_CMD_DESTROY:
            LOGW("APP_CMD_DESTROY");
            break;

        default:
            LOGE("Event code not handled: %d", cmd);
    }
}

bool Android_process_command()
{
    assert(AndroidUtils::nativeApplication != nullptr);

    int events;
    android_poll_source *source;

    // Poll all pending events.
    if (ALooper_pollAll(0, NULL, &events, (void **)&source) >= 0)
    {
        // Process each polled events
        if (source != NULL)
        {
            source->process(AndroidUtils::nativeApplication, source);
        }
    }

    return AndroidUtils::nativeApplication->destroyRequested == 0;
}

NATIVE_METHOD(void, nativeOnCreate)
(JNIEnv* env, jobject obj, jobject asset_mgr, jlong gvr_context_ptr)
{
    jlong j_native_gvr_context = gvr_context_ptr;
    gvr_context* context = reinterpret_cast<gvr_context *>(j_native_gvr_context);

    AndroidUtils::gvrApi = gvr::GvrApi::WrapNonOwned(context);
    AndroidUtils::gvrContext = context;

    AndroidUtils::InitControllerApi();

    VK_LOG(LogAndroid, Debug, "GVR API INITIALIZED");
}

void android_main(struct android_app *app)
{
    // Set static variables.
    AndroidUtils::nativeApplication = app;

    // Set the callback to process system events
    app->onAppCmd = Android_handle_cmd;

    // Forward cout/cerr to logcat.
    std::cout.rdbuf(new AndroidBuffer(ANDROID_LOG_INFO));
    std::cerr.rdbuf(new AndroidBuffer(ANDROID_LOG_ERROR));

    vulkan_android_main(0, nullptr);

    // Main loop
    do {
        if (!Android_process_command())
        {
            break;
        }

        AndroidUtils::Update();

        if (AndroidUtils::vulkanApplication && AndroidUtils::vulkanApplication->IsPendingKill())
        {
            break;
        }

    }  // Check if system requested to quit the application
    while (app->destroyRequested == 0);

    AndroidUtils::Clean();

    return;
}

ANativeWindow *AndroidGetApplicationWindow() {
    assert(AndroidUtils::nativeApplication);
    return AndroidUtils::nativeApplication->window;
}

bool AndroidLoadFile(const char *filePath, std::string *data) {
    assert(AndroidUtils::nativeApplication);
    AAsset *file = AAssetManager_open(AndroidUtils::nativeApplication->activity->assetManager, filePath, AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);
    LOGI("Loaded file:%s size:%zu", filePath, fileLength);
    if (fileLength == 0) {
        return false;
    }
    data->resize(fileLength);
    AAsset_read(file, &(*data)[0], fileLength);
    return true;
}

void AndroidGetWindowSize(int32_t& width, int32_t& height) {
    // On Android, retrieve the window size from the native window.
    assert(AndroidUtils::nativeApplication);

    width = ANativeWindow_getWidth(AndroidUtils::nativeApplication->window);
    height = ANativeWindow_getHeight(AndroidUtils::nativeApplication->window);
}

// Android fopen stub described at
// http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/#comment-1850768990
static int android_read(void *cookie, char *buf, int size) { return AAsset_read((AAsset *)cookie, buf, size); }

static int android_write(void *cookie, const char *buf, int size) {
    return EACCES;  // can't provide write access to the apk
}

static fpos_t android_seek(void *cookie, fpos_t offset, int whence) { return AAsset_seek((AAsset *)cookie, offset, whence); }

static int android_close(void *cookie) {
    AAsset_close((AAsset *)cookie);
    return 0;
}

FILE *AndroidFopen(const char *fname, const char *mode) {
    if (mode[0] == 'w') {
        return NULL;
    }

    assert(AndroidUtils::nativeApplication);
    AAsset *asset = AAssetManager_open(AndroidUtils::nativeApplication->activity->assetManager, fname, 0);
    if (!asset) {
        return NULL;
    }

    return funopen(asset, android_read, android_write, android_seek, android_close);
}

#endif

// AndroidUtils static class definitions.
#ifdef __ANDROID__

std::unique_ptr<vulkan::Application> AndroidUtils::vulkanApplication(nullptr);
android_app* AndroidUtils::nativeApplication = nullptr;

gvr::ControllerApi* AndroidUtils::controllerApi = nullptr;
std::unique_ptr<gvr::GvrApi> AndroidUtils::gvrApi(nullptr);
gvr_context* AndroidUtils::gvrContext = nullptr;

bool AndroidUtils::isPaused = true;

void AndroidUtils::Init()
{
    if (vulkanApplication)
    {
        vulkanApplication->Init();
        if (vulkanApplication->GetEngine())
        {
            vulkanApplication->GetEngine()->RequestPerformanceDataLog();
        }
    }
}

void AndroidUtils::Close()
{
    Clean();
}

void AndroidUtils::Start()
{
    isPaused = false;
}

void AndroidUtils::Update()
{
    if (vulkanApplication && vulkanApplication->GetEngine() && !isPaused)
    {
        if (controllerApi)
        {
            gvr::ControllerState controllerState;
            controllerState.Update(*controllerApi);
            if (controllerState.GetConnectionState() == gvr::ControllerConnectionState::GVR_CONTROLLER_DISCONNECTED)
            {
                InitControllerApi();
            }
        }
        else
        {
            InitControllerApi();
        }

        vulkanApplication->GetEngine()->GetInputManager()->UpdateGVRControllerState(controllerApi);
        vulkanApplication->GetEngine()->GetInputManager()->UpdateGVRHeadPose(gvrApi.get());
        vulkanApplication->GetEngine()->Update();
    }
}

void AndroidUtils::Pause()
{
    isPaused = true;
}

void AndroidUtils::Resume()
{
    isPaused = false;
}

void AndroidUtils::Clean()
{
    isPaused = true;

    Application* application = vulkanApplication.release();
    if (application)
    {
        delete application;
    }
}

void AndroidUtils::InitControllerApi()
{
    int32_t options = gvr::ControllerApi::DefaultOptions();

    // If you need accelerometer and gyro, enable them (they are not on
    // by default). Touch, buttons and orientation are on by default.
    options |= GVR_CONTROLLER_ENABLE_GYRO;
    options |= GVR_CONTROLLER_ENABLE_ACCEL;

    // Create the ControllerApi and ControllerState objects.
    // controller_api = std::unique_ptr<gvr::ControllerApi>(new gvr::ControllerApi);
    if (!controllerApi)
    {
        AndroidUtils::controllerApi = new gvr::ControllerApi;
    }

    // Initialize it. Notice that we pass the gvr_context pointer.
    if (!controllerApi->Init(options, gvrContext)) {
        // Handle failure. Do not proceed in case of failure (calling other
        // controller_api methods without a successful Init will crash with
        // an assert failure.
        return;
    }

    // If your app is in the resumed state (the Activity got onResume()), resume
    // the controller API now (if not, wait until you get onResume()).
    controllerApi->Resume();

    VK_LOG(LogAndroid, Debug, "GVR CONTROLLER INITIALIZED");
}

// #TODO Refactor variable names
std::vector<char> AndroidUtils::GetFileStream(std::string fileToFind)
{
    AAssetManager * asset_manager = nativeApplication->activity->assetManager;
    AAssetDir* assetDir = AAssetManager_openDir(asset_manager, "");
    const char* filename;
    std::vector<char> buffer;

    while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
    {
        if(!strcmp(filename, fileToFind.c_str()))
        {
            AAsset *asset = AAssetManager_open(asset_manager, filename, AASSET_MODE_STREAMING);

            //holds size of searched file
            off64_t length = AAsset_getLength64(asset);
            //keeps track of remaining bytes to read
            off64_t remaining = AAsset_getRemainingLength64(asset);
            size_t Mb = 1000 *1024; // 1Mb is maximum chunk size for compressed assets
            size_t currChunk;
            buffer.reserve(length);

            //while we have still some data to read
            while (remaining != 0)
            {
                //set proper size for our next chunk
                if(remaining >= Mb)
                {
                    currChunk = Mb;
                }
                else
                {
                    currChunk = remaining;
                }
                char chunk[currChunk];

                //read data chunk
                if(AAsset_read(asset, chunk, currChunk) > 0) // returns less than 0 on error
                {
                    //and append it to our vector
                    buffer.insert(buffer.end(),chunk, chunk + currChunk);
                    remaining = AAsset_getRemainingLength64(asset);
                }
            }
            AAsset_close(asset);

//            std::istream is(&databuf);
//            filestream(&databuf);
        }
    }

    AAssetDir_close(assetDir);

    return buffer;
}

#endif
