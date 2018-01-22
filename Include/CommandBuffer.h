/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file CommandBuffer.h
 */

VULKAN_NS_BEGIN

/**
 * @class CommandBuffer
 */
class CommandBuffer
{
public:
    /**
     * Default constructor.
     */
    CommandBuffer() = default;

    /**
     * Default destructor.
     */
    ~CommandBuffer() = default;

    VkCommandBuffer GetVkCommandBuffer();
    VkCommandBuffer* GetVkCommandBufferPtr();

    //void Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo inheritanceInfo);
    void Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* inheritanceInfo);
    void Reset(VkCommandBufferResetFlags flags);
    void End();

    void BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, std::vector<VkClearValue> clearValues, VkSubpassContents subpassContents);
    void EndRenderPass();

    void CopyBufferData(VkBuffer srcBuffer, VkBuffer dstBuffer, std::vector<VkBufferCopy> regions);
    void CopyBufferData(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size);

    void CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout layout, std::vector<VkBufferImageCopy> regions);
    void CopyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, std::vector<VkBufferImageCopy> regions);
    void CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, std::vector<VkImageCopy> regions);

    void SetPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
        std::vector<VkMemoryBarrier> memoryBarriers, std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers, std::vector<VkImageMemoryBarrier> imageMemoryBarriers);

    void FillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data);

    template<typename T>
    void FillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, const T value)
    {
        vkCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, *(const uint32_t*)&value);
    }

    void UpdateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint32_t* data);

    void ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue color, std::vector<VkImageSubresourceRange> ranges);
    void ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue depthStencil, std::vector<VkImageSubresourceRange> ranges);

    void BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, std::vector<VkImageBlit> regions, VkFilter filter);
private:
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

VULKAN_NS_END