/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Buffer.h"

/**
 * @file Renderer.h
 */

VULKAN_NS_BEGIN

/*
 * @class Renderer
 */
class Renderer
{
public:
    /**
     * Default constructor.
     */
    Renderer() = default;

    /**
     * Default destructor.
     */
    ~Renderer() = default;

protected:
    Buffer uniformBuffer;
};

VULKAN_NS_END