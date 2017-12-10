#include "VulkanCore.h"

VULKAN_NS_USING;

bool VulkanObject::IsDirty() const
{
    return bIsDirty;
}

void VulkanObject::SetDirty(bool bDirtyFlag)
{
    bIsDirty = bDirtyFlag;
}
