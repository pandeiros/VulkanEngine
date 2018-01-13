/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include <chrono>

/**
 * @file World.h
 */

VULKAN_NS_BEGIN

/**
 * @class World
 */
class World
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

    //////////////////////////////////////////////////////////////////////////
    // World management
    //////////////////////////////////////////////////////////////////////////

    void Create();
    void Update(float deltaTime);

private:

    //////////////////////////////////////////////////////////////////////////
    // Objects
    //////////////////////////////////////////////////////////////////////////

public:
    //std::vector<VulkanObject
};

VULKAN_NS_END