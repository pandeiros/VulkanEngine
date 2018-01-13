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
class Application : public VulkanObject
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

    virtual void Init() override;
    virtual void Destroy() override;

    Instance& GetInstanceRef();

protected:
    Instance instance;
    Engine engine;

private:
    VkApplicationInfo applicationInfo = {};
};

VULKAN_NS_END