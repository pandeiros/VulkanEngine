/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Instance.h"

/**
 * @file Application.h
 */

VULKAN_NS_BEGIN

/**
 * @class Application
 */
class Application
{
public:
    /**
     * Default constructor.
     */
    Application() = default;

    /**
     * Default destructor.
     */
    ~Application() = default;

    void Create(const char* applicationName, uint32_t applicationVersion, uint32_t apiVersion);

    void Init();

    void Destroy();

    Instance& GetInstanceRef();

private:
    VkApplicationInfo applicationInfo = {};

    Instance instance;
    Engine engine;
};

VULKAN_NS_END