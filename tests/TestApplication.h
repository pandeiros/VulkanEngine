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

#ifdef __ANDROID__
#include "gvr.h"
#include "gvr_controller.h"
#endif

#include <chrono>

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
    TestApplication();

    ~TestApplication();

    void Init() override;

protected:
    void Tick(float deltaTime) override;

private:
    CommandPool commandPool;
    Camera* camera;

    // Test variables.
    float colorRotator = 0.f;
    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point lastTime;

    // Perlin noise
    uint32_t cubesToDraw = 0;
    const uint32_t PERLIN_NODE_COUNT = 20;
    //float PerlinGradientMap[]

#ifdef __ANDROID__
    // GVR Controller
    gvr::ControllerState controller_state;
#endif
};

VULKAN_NS_END