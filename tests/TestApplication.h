/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Application.h"
#include "Buffer.h"
#include "CommandPool.h"
#include "Actor.h"

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
};

/**
 * @class Actor
 */
class TestActor : public Actor
{
public:
    virtual void Tick(float deltaTime);

    void OnModeChange(InputCode inputCode, InputEvent event, float value);

    glm::vec3 originPos;
    glm::vec3 destPos;

    bool bAnimSwitch = false;

    float timer = 0.f;
};

VULKAN_NS_END