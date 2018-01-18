#pragma once

#include <CommandPool.h>
#include <Application.h>
#include <Buffer.h>

#include "gvr.h"
#include "gvr_controller.h"

#include <chrono>

class AndroidApplication : public vulkan::Application
{
public:
    AndroidApplication()
            : Application("Vulkan Engine App Test", 1, VK_MAKE_VERSION(1, 0, 2))
    {}

    ~AndroidApplication();

    virtual void Init() override;
//    virtual void Destroy() override;

protected:
    virtual void Tick(float deltaTime) override;

private:
    vulkan::CommandPool commandPool;
    VkSemaphore semaphoreRenderComplete;
    vulkan::Camera* camera;

    // Test variables.
    float colorRotator = 0.f;
    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point lastTime;

    // GVR Controller
    gvr::ControllerState controller_state;
};
