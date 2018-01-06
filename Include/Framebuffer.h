/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file Framebuffer.h
 */

VULKAN_NS_BEGIN

struct FramebufferDimensions
{
    uint32_t width;
    uint32_t height;
    uint32_t layers;
};

/**
 * @class Framebuffer
 */
class Framebuffer
{
public:
    /**
     * Default constructor.
     */
    Framebuffer() = default;

    /**
     * Default destructor.
     */
    ~Framebuffer() = default;

    void Create(VkDevice device, VkFramebufferCreateFlags flags, VkRenderPass renderPass,
        std::vector<VkImageView> attachments, FramebufferDimensions dimensions);

    void Destroy(VkDevice device);

    VkFramebuffer GetVkFramebuffer() const;

protected:
    VkFramebuffer framebuffer = VK_NULL_HANDLE;

    VkFramebufferCreateInfo framebufferCreateInfo = {};
};

VULKAN_NS_END