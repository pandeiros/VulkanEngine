/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file Object.h
 */

VULKAN_NS_BEGIN

/**
 * @class Object
 */
class Object
{
public:
    virtual void Init() {};
    virtual void Destroy() {};
    void Update(float deltaTime);

    bool IsDirty() const;
    void SetDirty(bool isDirty);

    void SetUpdateEnabled(bool isUpdateEnabled);

    bool IsPendingKill() const;
    void SetPendingKill(bool pendingKill);

protected:
    virtual void Tick(float deltaTime) {};

private:
    bool bIsDirty = false;
    bool bIsUpdateEnabled = false;
    bool bPendingKill = false;
};

VULKAN_NS_END