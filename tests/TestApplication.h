/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Application.h"
#include "Buffer.h"
#include "CommandPool.h"

/**
 * @file TestApplication.h
 */

VULKAN_NS_BEGIN

/**
 * @class TestApplication
 */
class TestApplication : public Application
{
public:
    TestApplication()
        : Application("Vulkan Engine App Test", 1, VK_MAKE_VERSION(1, 0, 2))
    {}

    ~TestApplication();


    void Init() override;
    void Tick(float deltaTime) override;

private:
    CommandPool commandPool;
    VkSemaphore semaphoreRenderComplete;
    Camera* camera;
};

VULKAN_NS_END