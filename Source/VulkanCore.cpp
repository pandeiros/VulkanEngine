#include "VulkanCore.h"

VULKAN_NS_USING;

void VulkanObject::Update()
{
    if (bIsUpdateEnabled)
    {
        this->Tick();
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
