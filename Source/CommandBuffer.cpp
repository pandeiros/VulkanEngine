/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "CommandBuffer.h"
#include "Core.h"

VULKAN_NS_USING;

VkCommandBuffer CommandBuffer::GetVkCommandBuffer()
{
    return commandBuffer;
}

VkCommandBuffer& CommandBuffer::GetVkCommandBufferRef()
{
    return commandBuffer;
}

//void CommandBuffer::Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo inheritanceInfo)
//{
//    const VkCommandBufferBeginInfo commandBufferBeginInfo = {
//        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
//        nullptr,
//        flags,
//        &inheritanceInfo
//    };
//
//    VK_VERIFY(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
//}

void CommandBuffer::Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* inheritanceInfo)
{
    const VkCommandBufferBeginInfo commandBufferBeginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        flags,
        inheritanceInfo
    };

    VK_VERIFY(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
}

void CommandBuffer::Reset(VkCommandBufferResetFlags flags)
{
    VK_VERIFY(vkResetCommandBuffer(commandBuffer, flags));
}

void CommandBuffer::End()
{
    VK_VERIFY(vkEndCommandBuffer(commandBuffer));
}

void CommandBuffer::BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, std::vector<VkClearValue> clearValues, VkSubpassContents subpassContents)
{
    VkRenderPassBeginInfo renderPassBeginInfo = {
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        nullptr,
        renderPass,
        framebuffer,
        renderArea,
        (uint32_t)clearValues.size(),
        clearValues.data()
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, subpassContents);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void CommandBuffer::CopyBufferData(VkBuffer srcBuffer, VkBuffer dstBuffer, std::vector<VkBufferCopy> regions)
{
    // #TODO Check data beforehand (page 104 from Vulkan book).
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, (uint32_t)regions.size(), regions.data());
}

void CommandBuffer::CopyBufferData(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size)
{
    const VkBufferCopy region = { srcOffset, dstOffset, size };
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &region);
}

void CommandBuffer::CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout layout, std::vector<VkBufferImageCopy> regions)
{
    vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, layout, (uint32_t)regions.size(), regions.data());
}

void CommandBuffer::CopyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, std::vector<VkBufferImageCopy> regions)
{
    vkCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, (uint32_t)regions.size(), regions.data());
}

void CommandBuffer::CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, std::vector<VkImageCopy> regions)
{
    vkCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, (uint32_t)regions.size(), regions.data());
}

void CommandBuffer::SetPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, std::vector<VkMemoryBarrier> memoryBarriers, std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers, std::vector<VkImageMemoryBarrier> imageMemoryBarriers)
{
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, (uint32_t)memoryBarriers.size(), memoryBarriers.data(),
        (uint32_t)bufferMemoryBarriers.size(), bufferMemoryBarriers.data(), (uint32_t)imageMemoryBarriers.size(), imageMemoryBarriers.data());
}

void CommandBuffer::FillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
    vkCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}

void CommandBuffer::UpdateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint32_t* data)
{
    vkCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, data);
}

void CommandBuffer::ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue color, std::vector<VkImageSubresourceRange> ranges)
{
    vkCmdClearColorImage(commandBuffer, image, imageLayout, &color, (uint32_t)ranges.size(), ranges.data());
}

void CommandBuffer::ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue depthStencil, std::vector<VkImageSubresourceRange> ranges)
{
    vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, &depthStencil, (uint32_t)ranges.size(), ranges.data());
}

void CommandBuffer::BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, std::vector<VkImageBlit> regions, VkFilter filter)
{
    vkCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, (uint32_t)regions.size(), regions.data(), filter);
}
