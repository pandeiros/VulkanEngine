/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file RenderPass.h
 */

VULKAN_NS_BEGIN

/**
 * @class RenderPass
 */
class RenderPass
{
public:
    /**
     * Default constructor.
     */
    RenderPass() = default;

    /**
     * Default destructor.
     */
    ~RenderPass() = default;

    void Create(VkDevice device, VkRenderPassCreateFlags flags, std::vector<VkAttachmentDescription> attachments,
        std::vector<VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> dependencies);

    void Destroy(VkDevice device);

    VkRenderPass GetVkRenderPass() const;

protected:
    VkRenderPass renderPass;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
};

VULKAN_NS_END