#pragma once

#include <CommandPool.h>
#include <Application.h>

class AndroidApplication : public vulkan::Application
{
public:
    virtual void Init() override;
    virtual void Update() override;
    virtual void Destroy() override;

private:
    vulkan::CommandPool commandPool;
    VkSemaphore semaphoreRenderComplete;
};
