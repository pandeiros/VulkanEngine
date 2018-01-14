/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "VulkanCore.h"

VULKAN_NS_USING;

void VulkanClass::Update(float deltaTime)
{
    if (bIsUpdateEnabled)
    {
        this->Tick(deltaTime);
    }
}

bool VulkanClass::IsDirty() const
{
    return bIsDirty;
}

void VulkanClass::SetDirty(const bool isDirty)
{
    bIsDirty = isDirty;
}

void VulkanClass::SetUpdateEnabled(const bool isUpdateEnabled)
{
    bIsUpdateEnabled = isUpdateEnabled;
}

bool VulkanClass::IsPendingKill() const
{
    return bPendingKill;
}

void VulkanClass::SetPendingKill(const bool pendingKill)
{
    bPendingKill = pendingKill;
}
