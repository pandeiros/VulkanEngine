/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Object.h"
#include "Camera.h"

#include <chrono>
#include <set>

/**
 * @file World.h
 */

VULKAN_NS_BEGIN

/**
 * @class Actor
 */
class Actor : public Object
{
public:
    /**
     * Default constructor.
     */
    Actor() = default;

    /**
     * Default destructor.
     */
    ~Actor() = default;
};

/**
 * @class World
 */
class World : public VulkanClass
{
public:
    /**
     * Default constructor.
     */
    World() = default;

    /**
     * Default destructor.
     */
    ~World() = default;

    void Destroy() override;

    //////////////////////////////////////////////////////////////////////////
    // World management
    //////////////////////////////////////////////////////////////////////////

    void Tick(float deltaTime) override;

private:

    //////////////////////////////////////////////////////////////////////////
    // Camera
    //////////////////////////////////////////////////////////////////////////

public:
    void AddCamera(Camera* camera);

private:
    std::set<Camera*> cameras;

    //////////////////////////////////////////////////////////////////////////
    // Objects
    //////////////////////////////////////////////////////////////////////////

public:
    std::vector<Object*> worldObjects;
};

VULKAN_NS_END