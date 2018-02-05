/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Object.h"
#include "Rendering/RenderComponent.h"

/**
 * @file Actor.h
 */

VULKAN_NS_BEGIN

/**
 * @class Actor
 */
class Actor
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

protected:
    RenderComponent renderComponent;
};

VULKAN_NS_END