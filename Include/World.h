/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Actor.h"
#include "Camera.h"
#include "Device.h"
#include "Buffer.h"

#include <chrono>
#include <set>

/**
 * @file World.h
 */

VULKAN_NS_BEGIN

class Engine;

/**
 * @class World
 */
class World : public VulkanClass
{
public:
    World(DevicePtr device, Engine* engine);

    ~World();

private:
    Engine* engine;

    //////////////////////////////////////////////////////////////////////////
    // World management
    //////////////////////////////////////////////////////////////////////////

public:
    void Tick(float deltaTime) override;

    bool PrepareVertexData(VertexBuffer& vertexBuffer, ShaderIndexData& shaderIndexData);

private:

    //////////////////////////////////////////////////////////////////////////
    // Camera
    //////////////////////////////////////////////////////////////////////////

public:
    glm::mat4 GetCameraMatrix(uint32_t viewportIndex);

    void SetCamera(CameraMode cameraMode, float yFovDegrees, float aspectRatio, float zNear, float zFar);

    //void AddCamera(Camera* camera);

private:
    std::vector<std::unique_ptr<Camera>> cameras;

    //////////////////////////////////////////////////////////////////////////
    // Actors
    //////////////////////////////////////////////////////////////////////////

public:
    void AddActor(Actor* actor);

private:
    std::vector<Actor*> actors;
};

VULKAN_NS_END