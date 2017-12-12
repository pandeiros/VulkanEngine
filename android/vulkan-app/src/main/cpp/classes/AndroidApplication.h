#pragma once

#include <CommandPool.h>
#include <Application.h>
#include "gvr.h"
#include "gvr_controller.h"

#include <chrono>

class AndroidApplication : public vulkan::Application
{
public:
    virtual void Init() override;
    virtual void Destroy() override;

protected:
    virtual void Tick() override;

private:
    vulkan::CommandPool commandPool;
    VkSemaphore semaphoreRenderComplete;

    // Test variables.
    float colorRotator = 0.f;
    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point lastTime;
    uint64_t frameCounter = 0;
    uint64_t FPS = 0;

    // GVR Controller
    gvr::ControllerState controller_state;
};
