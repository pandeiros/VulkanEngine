/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "RenderComponent.h"

/**
 * @file Shape.h
 */

VULKAN_NS_BEGIN

/**
 * @class Shape
 */
class Shape : public RenderComponent
{
public:
    /**
     * Default constructor.
     */
    Shape() = default;

    /**
     * Default destructor.
     */
    ~Shape() = default;
};

VULKAN_NS_END