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

    Actor* CreateActor(std::string filename);

    void ColorTreeVertices(RenderComponent* renderComponent, std::string filename);
    void ColorSun(RenderComponent* renderComponent, std::vector<glm::vec3>& normals);
    void ShadeFaces(RenderComponent* renderComponent, std::vector<glm::vec3>& normals, float factor = 4.f);
};

/**
 * @class TestCube
 */
class TestCube : public Actor
{
public:
    virtual void Tick(float deltaTime);

    void OnModeChange(InputCode inputCode, InputEvent event, float value);

    glm::vec3 originPos;
    glm::vec3 destPos;

    bool bAnimSwitch = false;

    float timer = 0.f;
};

/**
* @class TestSceneComponent
*/
class TestSceneComponent : public SceneComponent
{
public:
    virtual void UpdateData();

    virtual void SetColor(const glm::vec3 color);
};

/**
 * @class Cloud
 */
class TestCloud : public Actor
{
public:
    virtual void Tick(float deltaTime);

private:
    float speed = 0.f;
};


VULKAN_NS_END