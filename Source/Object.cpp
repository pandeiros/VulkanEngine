/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Object.h"

VULKAN_NS_USING;

void Object::Update(float deltaTime)
{
    if (bIsUpdateEnabled)
    {
        this->Tick(deltaTime);
    }
    else
    {
        SetDirty(false);
    }
}

bool Object::IsDirty() const
{
    return bIsDirty;
}

void Object::SetDirty(const bool isDirty)
{
    bIsDirty = isDirty;
}

void Object::SetUpdateEnabled(const bool isUpdateEnabled)
{
    bIsUpdateEnabled = isUpdateEnabled;
}

bool Object::IsPendingKill() const
{
    return bPendingKill;
}

void Object::SetPendingKill(const bool pendingKill)
{
    bPendingKill = pendingKill;
}