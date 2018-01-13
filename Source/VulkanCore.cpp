/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "VulkanCore.h"

VULKAN_NS_USING;

void VulkanObject::Update(float deltaTime)
{
    if (bIsUpdateEnabled)
    {
        this->Tick(deltaTime);
    }
}

bool VulkanObject::IsDirty() const
{
    return bIsDirty;
}

void VulkanObject::SetDirty(const bool isDirty)
{
    bIsDirty = isDirty;
}

void VulkanObject::SetUpdateEnabled(const bool isUpdateEnabled)
{
    bIsUpdateEnabled = isUpdateEnabled;
}

bool VulkanObject::IsPendingKill() const
{
    return bPendingKill;
}

void VulkanObject::SetPendingKill(const bool pendingKill)
{
    bPendingKill = pendingKill;
}
