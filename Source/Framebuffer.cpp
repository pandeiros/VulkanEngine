/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Framebuffer.h"
#include "Core.h"

VULKAN_NS_USING;

void Framebuffer::Create(VkDevice device, VkFramebufferCreateFlags flags, VkRenderPass renderPass, std::vector<VkImageView> attachments, FramebufferDimensions dimensions)
{
    framebufferCreateInfo = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        nullptr,
        0,
        renderPass,
        (uint32_t)attachments.size(),
        attachments.data(),
        dimensions.width,
        dimensions.height,
        dimensions.layers
    };

    DebugTools::Verify(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer));
}

void Framebuffer::Destroy(VkDevice device)
{
    if (device)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    framebuffer = VK_NULL_HANDLE;
}

VkFramebuffer Framebuffer::GetVkFramebuffer() const
{
    return framebuffer;
}
