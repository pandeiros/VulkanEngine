/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "RenderPass.h"
#include "Core.h"

VULKAN_NS_USING;

void RenderPass::Create(VkDevice device, VkRenderPassCreateFlags flags, std::vector<VkAttachmentDescription> attachments,
    std::vector<VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> dependencies)
{
    renderPassCreateInfo = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)attachments.size(),
        attachments.data(),
        (uint32_t)subpasses.size(),
        subpasses.data(),
        (uint32_t)dependencies.size(),
        dependencies.data()
    };

    VK_VERIFY(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));
}

void RenderPass::Destroy(VkDevice device)
{
    if (device)
    {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }

    renderPass = VK_NULL_HANDLE;
}

VkRenderPass RenderPass::GetVkRenderPass() const
{
    return renderPass;
}
