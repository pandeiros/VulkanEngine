/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Instance.h"
#include "Engine.h"

/**
 * @file Application.h
 */

VULKAN_NS_BEGIN

/**
 * @class Application
 */
class Application : public VulkanClass
{
public:
    Application(const char* applicationName, uint32_t applicationVersion, uint32_t apiVersion);

    /**
     * Default destructor.
     */
    ~Application();

    virtual void Init() override;

    Instance* GetInstance();
    Engine* GetEngine();

protected:
    std::unique_ptr<Instance> instance;
    std::unique_ptr<Engine> engine;

private:
    VkApplicationInfo applicationInfo = {};
};

VULKAN_NS_END